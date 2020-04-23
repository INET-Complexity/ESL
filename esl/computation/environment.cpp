/// \file   environment.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-11-24
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
///             Oxford Martin School, University of Oxford
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
#include <sstream>
#include <fstream>
#include <chrono>
using std::chrono::high_resolution_clock;

#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <esl/agent.hpp>
#include <esl/computation/environment.hpp>
#include <esl/simulation/model.hpp>
#include <esl/economics/price.hpp>
#include <esl/data/serialization.hpp>
#include <esl/data/log.hpp>


namespace esl::computation {

    environment::environment()
    {

    }

    ///
    /// agents are activated when they are newly created or when they migrate
    /// from one computational process to another
    ///
    size_t environment::activate()
    {
        size_t result_ = 0;
        for(auto a : activated_) {
            activate_agent(a);
            ++result_;
        }
        activated_.clear();
        return result_;
    }

    ///
    /// \brief  Agents are deactivated when they are deleted
    ///
    size_t environment::deactivate()
    {
        size_t result_ = 0;
        for(auto a : deactivated_) {
            deactivate_agent(a);
            ++result_;
        }
        return result_;
    }

    void environment::before_step()
    {

    }

    void environment::after_step(simulation::model &simulation)
    {
        (void) simulation;
    }

    void environment::after_run(simulation::model &simulation)
    {
        return;
        for(auto &[i, a] : simulation.agents.local_agents_) {
            for(auto o: a->outputs){
                std::stringstream filename_;
                filename_ << i << '_' << o.first << ".xml";
                std::cout << filename_.str() << std::endl;
                std::ofstream ofs(filename_.str());
                LOG(trace) << "output " << o.second->name << " to file " << filename_.str() << std::endl;
                assert(ofs.good());
                boost::archive::xml_oarchive oa(ofs);
                oa.template register_type<esl::economics::price>();
                oa.template register_type<std::vector<esl::economics::price>>();
                typedef std::tuple<esl::simulation::time_point, std::vector<esl::economics::price>> tuple_time_point_price_vector;
                oa.template register_type<tuple_time_point_price_vector>();
                typedef std::vector<std::tuple<esl::simulation::time_point, std::vector<esl::economics::price>>> time_series_price_vector;
                oa.template register_type<time_series_price_vector>();
                oa.template register_type<esl::data::output<std::vector<esl::economics::price>>>();
                esl::data::output_base *ptr = o.second.get();
                oa << boost::serialization::make_nvp("output", ptr);
            }
        }
    }

    ///
    /// Send and receive messages, to all nodes all at the same time
    ///
    size_t environment::send_messages(simulation::model &simulation)
    {
        size_t messages_ = 0;
        for(auto &[i, a] : simulation.agents.local_agents_) {
            (void) i;
            for(auto m : a->outbox) {
                auto iterator_ =
                    simulation.agents.local_agents_.find(m->recipient);
                if(simulation.agents.local_agents_.end() == iterator_) {
                    // not in distributed mode, and no local agent matching
                    throw std::logic_error("agent not found "
                                           + m->recipient.representation());
                }

                iterator_->second->inbox.insert({m->received, m});
                ++messages_;
            }
            a->outbox.clear();
        }

        return messages_;
    }


    ///
    /// \param a
    void environment::activate_agent(const identity<agent> &a)
    {
        activated_.push_back(a);
    }

    ///
    /// \param a
    void environment::deactivate_agent(const identity<agent> &a)
    {
        // agent_action_time_.erase(a);
        deactivated_.push_back(a);
    }


    ///
    /// \param simulation
    void environment::step(simulation::model &simulation)
    {
        simulation::time_interval step_ = {simulation.time, simulation.end};
        if(simulation.time < simulation.end) {
            simulation.step(step_);
        }

        size_t changes_ = 0;
        changes_ += activate();
        changes_ += deactivate();
    }

    ///
    /// \param simulation
    void environment::run(simulation::model &simulation)
    {
        auto timer_start_run_ = high_resolution_clock::now();
        simulation.initialize();
        auto timer_initialization_ = high_resolution_clock::now() - timer_start_run_;

        simulation::time_interval step_ = {simulation.start, simulation.end};
        do {
            size_t changes_ = 0;
            changes_ += activate();
            changes_ += deactivate();

            step_.lower = simulation.step(step_);
        } while(step_.lower < simulation.end);

        auto timer_simulation_ = high_resolution_clock::now() - timer_start_run_;
        std::cout << "simulation took " << (double(timer_simulation_.count()) / 1e+9) <<  " seconds" << std::endl;


        simulation.terminate();

        auto timer_termination_ = high_resolution_clock::now() - timer_simulation_;

        after_run(simulation);

        auto timer_processing_after_ = high_resolution_clock::now() - timer_termination_;

        auto timer_total_ = high_resolution_clock::now() - timer_start_run_;
        std::cout << "running simulation in " << typeid(decltype(*this)).name() << " took " << (double(timer_total_.count()) / 1e+9) <<  " seconds" << std::endl;
    }
}// namespace esl::computation


#ifdef WITH_PYTHON
#include <boost/python.hpp>
#include <boost/python/enum.hpp>
#include <esl/interaction/message.hpp>


namespace esl::computation {
    using namespace boost::python;
    BOOST_PYTHON_MODULE(environment)
    {
        class_<environment>("environment")
            .def("step", &environment::step)
            .def("run", &environment::run);
    }
}  // namespace esl::computation
#endif