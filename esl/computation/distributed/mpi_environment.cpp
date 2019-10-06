/// \file   parallel_environment.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-11-24
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
/// Oxford Martin School, University of Oxford
///
///             Licensed under the Apache License, Version 2.0 (the "License");
///             you may not use this file except in compliance with the License.
///             You may obtain a copy of the License at
///
///                 http://www.apache.org/licenses/LICENSE-2.0
///
///             Unless required by applicable law or agreed to in writing,
///             software distributed under the License is distributed on an "AS
///             IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
///             express or implied. See the License for the specific language
///             governing permissions and limitations under the License.
///
///             You may obtain instructions to fulfill the attribution
///             requirements in CITATION.cff
///
#include <esl/computation/distributed/mpi_environment.hpp>

#ifdef WITH_MPI
#include <vector>

#include <boost/serialization/shared_ptr.hpp>

#include <esl/agent.hpp>
#include <esl/computation/timing.hpp>
#include <esl/interaction/header.hpp>
#include <esl/simulation/model.hpp>


namespace {
    template<class SharedPointer>
    struct Holder
    {
        SharedPointer p;

        Holder(const SharedPointer &p) : p(p)
        {}
        Holder(const Holder &other) : p(other.p)
        {}
        Holder(Holder &&other) : p(std::move(other.p))
        {}

        void operator()(...)
        {
            p.reset();
        }
    };
}  // namespace

template<class T>
std::shared_ptr<T> to_std_ptr(const boost::shared_ptr<T> &p)
{
    typedef Holder<std::shared_ptr<T>> H;
    if(H *h = boost::get_deleter<H>(p)) {
        return h->p;
    } else {
        return std::shared_ptr<T>(p.get(), Holder<boost::shared_ptr<T>>(p));
    }
}

template<class T>
boost::shared_ptr<T> to_boost_ptr(const std::shared_ptr<T> &p)
{
    typedef Holder<boost::shared_ptr<T>> H;
    if(H *h = std::get_deleter<H>(p)) {
        return h->p;
    } else {
        return boost::shared_ptr<T>(p.get(), Holder<std::shared_ptr<T>>(p));
    }
}


namespace esl::computation::distributed {

    ///
    /// \details    Assumes BOOST_MPI_HAS_NOARG_INITIALIZATION, meaning the MPI
    ///             implementation is version 2 or higher and provides an
    ///             initialization function that
    ///
    mpi_environment::mpi_environment()
    : environment_(boost::mpi::threading::level::single, true)
    , communicator_()
    , agent_locations_()
    {}

    void mpi_environment::process_migrations(
        const std::vector<migration> &migrations)
    {
        for(const auto m : migrations) {
            agent_locations_[m.migrant] = m.target;
        }
    }


    std::vector<migration> mpi_environment::migrate_agents()
    {
        return {};
    }

    size_t mpi_environment::activate()
    {
        std::vector<std::vector<activation>> activated_locally_(
            communicator_.size());
        for(auto &node : activated_locally_) {
            node.reserve(activated_.size());
            for(auto &i : activated_) {
                node.push_back({communicator_.rank(), i});
            }
        }
        std::vector<std::vector<activation>> activations_stacked_;
        boost::mpi::all_to_all(communicator_, activated_locally_,
                               activations_stacked_);

        std::vector<activation> activations_;
        for(auto s : activations_stacked_) {
            activations_.insert(activations_.begin(), s.begin(), s.end());
        }

        size_t result_ = 0;
        for(auto a : activations_) {
            activate_agent(a.activated, a.location);
            ++result_;
        }

        if(result_) {
            // log() << activated_.size()  << "/" << result_ << " activated" <<
            // endl;
        }

        activated_.clear();
        return result_;
    }


    size_t mpi_environment::deactivate()
    {
        auto result_ = deactivated_.size();
        std::vector<deactivation> deactivated_locally_(deactivated_.size());
        for(size_t i = 0; i < deactivated_.size(); ++i) {
            deactivated_locally_[i] = {deactivated_[i]};
        }

        std::vector<deactivation> deactivations_;
        boost::mpi::all_to_all(communicator_, deactivated_locally_,
                               deactivations_);

        for(const auto &a : deactivations_) {
            deactivate_agent(a.deactivated);
        }
        return result_;
    }

    ///
    /// Agent migrations
    ///
    void mpi_environment::migrate(simulation::model &simulation,
                                  agent_timing &timing)
    {
        (void)timing;  // TODO: re-implemnt timings


        constexpr int root =
            0;  // TODO: get confirmation this is up to MPI spec

        std::vector<std::vector<migration>> proposed_(communicator_.size());
        if(simulation.time >= simulation.end) {
            if(root != communicator_.rank()) {
                for(auto &a : simulation.agents.local_agents_) {
                    proposed_[root].push_back(
                        {communicator_.rank(), root, a.first});
                }
            } else {
            }
        } else {
            auto idea_ = migrate_agents();
            for(const auto &m : idea_) {
                proposed_[m.target].push_back(m);
            }

            /*
            std::vector<::esl::computation::agent_timing> timings_;
            boost::mpi::all_gather<esl::computation::agent_timing>(
                communicator_, timing, timings_);
            auto mean_agent_act_time_ =
                accumulate(
                    timings_.begin(), timings_.end(), 0.0,
                    [](double a, const ::esl::computation::agent_timing &b) {
                        return a + b.acting.count();
                    })
                / communicator_.size();


            if(mean_agent_act_time_ * 1.0 < timing.acting.count()) {
                // log() << "did " << (timing.agent_computation / 1000000) <<
                // "ms, other processes took on average " <<
                // double(mean_agent_act_time_ / 1000000) << "ms" << endl;
                auto budget_ = std::min<double>(0.1 * timing.acting.count(),
                                                timing.acting.count()
                                                    - mean_agent_act_time_);

                std::set<node_identifier> offload_;
                for(node_identifier n = 0; n < communicator_.size(); ++n) {
                    if(communicator_.rank() == n) {
                        continue;
                    }
                    if(mean_agent_act_time_ > timings_[n].acting.count()) {
                        offload_.insert(n);
                    }
                }

                // by property of the mean computation time this is must be true
                // if there are two or more nodes in the cluster
                if(!offload_.empty()) {
                    // log() << " offload_.size() " << offload_.size() << endl;
                    size_t offloaded_ = 0;
                    for(auto &a : simulation.agents.local_agents_) {
                        if(budget_ < 0.0) {
                            break;
                        }
                        std::vector<std::tuple<node_identifier, long double>>
                            communication_costs_;
                        // for(const auto &candidate_ :
                        // communications_[a.first]){
                        //    communication_costs_.push_back({candidate_.first,
                        //    candidate_.second.value()});
                        //}
                        std::sort(
                            communication_costs_.begin(),
                            communication_costs_.end(),
                            [](const std::tuple<node_identifier, long double>
                                   &first,
                               const std::tuple<node_identifier, long double>
                                   &second) -> bool {
                                return std::get<1>(first) > std::get<1>(second);
                            });
                        for(auto candidate_ : communication_costs_) {
                            if(communicator_.rank() == get<0>(candidate_)) {
                                continue;
                            }
                            if(offload_.find(get<0>(candidate_))
                               != offload_.end()) {
                                ++offloaded_;
                                // log() << "offloaded agent with mean compute
                                // time " <<
                                // (agent_action_time_[a->identifier].value() /
                                // 1000000) << "ms" << endl;
                                proposed_[get<0>(candidate_)].push_back(
                                    {communicator_.rank(), get<0>(candidate_),
                                     a.first});
                                // budget_ -=
                                // agent_action_time_[a.first].value(); log() <<
                                // "wants to move agent " << a->identifier << "
                                // to node " << get<0>(candidate_) << ", one of
                                // "
                                // << offload_.size() << " node candidates" <<
                                // endl;
                                break;
                            }
                        }
                    }
                    // log() << "offloaded " << offloaded_ << endl;
                }
            }*/
        }

        // propositions are peer to peer
        std::vector<std::vector<migration>> propositions_;
        boost::mpi::all_to_all<std::vector<migration>>(
            communicator_, (proposed_), propositions_);

        // accepted transfers are public
        std::vector<migration> accepted_;
        std::vector<std::vector<migration>> accepted_v_(communicator_.size());
        for(auto v : propositions_) {
            for(auto p : v) {
                if(p.target == communicator_.rank()) {
                    for(auto &a : accepted_v_) {
                        a.push_back(p);
                    }
                }
            }

            accepted_.insert(accepted_.begin(), v.begin(), v.end());
        }

        std::vector<std::vector<migration>> migrations_;
        boost::mpi::all_to_all(communicator_, accepted_v_, migrations_);
        std::vector<migration> result1_;
        for(auto &d : migrations_) {
            result1_.insert(result1_.end(), d.begin(), d.end());
        }
        process_migrations(result1_);

        std::cout << communicator_.rank() << " execute migrations" << std::endl;

        for(const auto &m : result1_) {
            if(m.source == communicator_.rank()) {
                // log() << "sending agent to " << m.target << endl;
                // TODO: inefficient
                for(const auto &a : simulation.agents.local_agents_) {
                    if(a.first == m.migrant) {
                        boost::shared_ptr<agent> a2 = to_boost_ptr(a.second);
                        communicator_.send(m.target, 0, a2);
                        simulation.agents.local_agents_.erase(a.first);
                        break;
                    }
                }
            } else if(m.target == communicator_.rank()) {
                // log() << "receiving agent from " << m.source << endl;
                boost::shared_ptr<agent> migrant_;
                communicator_.recv(m.source, 0, migrant_);
                simulation.agents.local_agents_.insert(
                    {migrant_->identifier, to_std_ptr(migrant_)});
                for(node_identifier n = 0; n < communicator_.size(); ++n) {
                    // communications_[migrant_->identifier].insert({n,
                    // timer<mean>()});
                }

                if(simulation.time >= simulation.end) {
                    // write agent outputs to disk
                    // storage_.save(*migrant_);
                }
            }
        }
    }


    ///
    /// Send and receive messages, to all nodes all at the same time
    ///
    size_t mpi_environment::send_messages(simulation::model &simulation)
    {
        std::vector<std::vector<interaction::header>> messages_;
        std::vector<std::vector<interaction::header>> messages_sent_(
            communicator_.size());

        for(const auto &[i, a] : simulation.agents.local_agents_) {
            (void) i;
            for(const auto &m : a->outbox) {
                auto target_ = agent_locations_[m->recipient];
                if(communicator_.rank() == target_) {
                    simulation.agents.local_agents_.find(m->recipient)
                        ->second->inbox.insert({m->received, m});
                    std::cout << "same process fastpath" << std::endl;
                } else {
                    messages_sent_[target_].push_back(interaction::header(*m));
                    std::cout << "other process delivery" << std::endl;
                }
            }
            a->outbox.clear();
        }

        boost::mpi::all_to_all(communicator_, messages_sent_, messages_);
        for(node_identifier i = 0; i < node_identifier(messages_.size()); ++i) {
            if(messages_[i].empty()) {
                continue;
            }
            for(auto &m : messages_[i]) {
                (void) m;
                if(i == communicator_.rank()) {
                    for(node_identifier j = 0;
                        node_identifier(messages_sent_.size());
                        ++j) {
                        for(const auto &message_ : messages_sent_[j]) {
                            communicator_.send<interaction::header>(j, 0,
                                                                    message_);
                        }
                    }

                } else if(i == communicator_.rank()) {
                    // std::shared_ptr<interaction::header> message_;
                    // communicator_.recv(i, 0, message_);
                }
            }
        }

        return 0;
    }

    ///
    /// \param simulation
    void
    mpi_environment::clear_agents(std::shared_ptr<simulation::model> simulation)
    {
        simulation->agents.local_agents_.clear();
        agent_locations_.clear();
    }

    ///
    /// \param a
    void mpi_environment::activate_agent(const identity<agent> &a)
    {
        agent_locations_[a] = static_cast<unsigned int>(communicator_.rank());

        activated_.push_back(a);
        for(node_identifier n = 0; n < communicator_.size(); ++n) {
            // communications_[a].insert({n, timer<mean>()});
        }
    }

    ///
    ///
    /// \param a
    /// \param n
    void mpi_environment::activate_agent(const identity<agent> &a,
                                         node_identifier n)
    {
        agent_locations_[a] = n;
    }

    ///
    /// \param a
    void mpi_environment::deactivate_agent(const identity<agent> &a)
    {
        agent_locations_.erase(a);
        // agent_action_time_.erase(a);
        deactivated_.push_back(a);
    }


    void mpi_environment::before_step()
    {}

    void mpi_environment::after_step(simulation::model &simulation)
    {
        agent_timing timing_;
        migrate(simulation, timing_);
        std::cout << "after_step()" << std::endl;
    }

    ///
    /// \return
    bool mpi_environment::is_coordinator() const
    {
        return communicator_.rank() == 0;
    }

    ///
    /// \param simulation
    void mpi_environment::run(std::shared_ptr<simulation::model> simulation)
    {
        // timer<mean> timer_initialisation_;
        // timer_initialisation_.start();
        simulation->initialize();
        // timer_initialisation_.stop();
        // log() << "initialisation " << timer_initialisation_.value() /
        // std::chrono::high_resolution_clock::period::den << "s" << endl;

        // timer<mean> timer_run_;
        // timer_run_.start();
        // timer<mean> communication_pre_;
        simulation::time_point next_timestep_minimum_ = simulation->time;

        simulation::time_interval timestep_ = {next_timestep_minimum_,
                                               simulation->end};

        while(timestep_.lower < simulation->end) {
            timestep_.lower = simulation->step(timestep_);
            // we need a temporary value so that the input and output memory
            // addresses don't overlap
            {
                simulation::time_point lower_;
                boost::mpi::reduce(
                    communicator_, timestep_.lower, lower_,
                    boost::mpi::minimum<simulation::time_point>(), 0);
                boost::mpi::broadcast(communicator_, lower_, 0);
                timestep_.lower = lower_;
                simulation::time_point upper_;
                boost::mpi::reduce(
                    communicator_, timestep_.upper, upper_,
                    boost::mpi::minimum<simulation::time_point>(), 0);
                boost::mpi::broadcast(communicator_, upper_, 0);
                timestep_.upper = upper_;
            };

            if(timestep_.lower >= simulation->end) {
                break;
            }

            if(is_coordinator()) {
                std::cout << "----------- round " << timestep_ << "-----------"
                          << std::endl;
            }

            next_timestep_minimum_ =
                simulation->time;  // by default, we progress to the end of the
                                   // current interval

            // communication_pre_.start();
            agent_timing timing_;

            size_t changes_ = 0;
            changes_ += activate();
            changes_ += deactivate();
            if(changes_) {
                // log() << "now has " << simulation->agents.agents_.size() << "
                // agents " << std::endl;
            }

            ////////////////////////////////////////////////////////////////////
            // report agent totals
            ////////////////////////////////////////////////////////////////////
            uint64_t agents_ = 0;
            {
                std::vector<uint64_t> agent_counts_;
                boost::mpi::all_gather(communicator_,
                                       simulation->agents.local_agents_.size(),
                                       agent_counts_);
                for(const auto &c : agent_counts_) {
                    agents_ += c;
                }
            }
            ////////////////////////////////////////////////////////////////////
            std::unordered_map<std::shared_ptr<agent>, bool> agent_done_;
            for(auto a : simulation->agents.local_agents_) {
                // agent_done_[a] = false;
            }

            simulation->step(timestep_);
        }

        // timer_run_.stop();
        // log() << "computation " << timer_run_.value()  /
        // std::chrono::high_resolution_clock::period::den << "s" << endl;
    }
}  // namespace esl::computation::distributed

#endif