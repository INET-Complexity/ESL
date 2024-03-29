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

#if BOOST_VERSION >= 106500
#include <boost/serialization/unordered_map.hpp>
#endif

#include <boost/core/demangle.hpp>

#include <esl/agent.hpp>
#include <esl/computation/environment.hpp>
#include <esl/simulation/model.hpp>
using esl::simulation::time_point;
#include <esl/economics/price.hpp>
using esl::economics::price;
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
        for(const auto &a: activated_) {
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
        for(const auto &a: deactivated_) {
            deactivate_agent(a);
            ++result_;
        }
        return result_;
    }

    ///
    /// \brief
    ///
    void environment::before_step()
    {

    }

    void environment::after_step(simulation::model &simulation)
    {
        (void) simulation;
    }

    void environment::after_run(simulation::model &simulation)
    {
        return; /// TODO
        for(auto &[i, a]: simulation.agents.local_agents_) {
            for(const auto &o: a->outputs){
                std::stringstream filename_;
                filename_ << i << '_' << o.first << ".xml";
                
                try{
                    std::ofstream ofs(filename_.str());
                    LOG(trace) << "output " << o.second->name << " to file "
                               << filename_.str() << std::endl;
                    assert(ofs.good());
                    boost::archive::xml_oarchive oa(ofs);
                    oa.template register_type<price>();
                    oa.template register_type<std::vector<price>>();
                    typedef std::tuple<time_point, std::vector<price>>
                        tuple_time_point_price_vector;
                    oa.template register_type<tuple_time_point_price_vector>();
                    typedef std::vector<
                        std::tuple<time_point, std::vector<price>>>
                        time_series_price_vector;
                    oa.template register_type<time_series_price_vector>();
                    oa.template register_type<
                        data::output<std::vector<price>>>();
                    data::output_base *ptr = o.second.get();
                    oa << boost::serialization::make_nvp("output", ptr);
                }catch(std::exception &e){
                    (void)e;
                }
            }
        }
    }

    ///
    /// Send and receive messages, to all nodes all at the same time
    ///
    size_t environment::send_messages(simulation::model &simulation)
    {
        size_t messages_ = 0;

        // agent locality refers to memory locality. In the multi-threaded
        // setting, we are still able to observe all agents from one thread
        for(auto &[i, a] :simulation.agents.local_agents_){
            (void)i;
            for(const auto &m :a->outbox){
                auto iterator_ = simulation.agents.local_agents_.find(m->recipient);
                if(simulation.agents.local_agents_.end() == iterator_) {
                    // We are not in distributed mode, and no local agent matching recipient.
                    // The only possible explanation is that the recipient identity is incorrect
                    throw esl::exception("message recipient agent not found " + m->recipient.representation());
                }
                // insert into inbox
                iterator_->second->inbox.insert({m->received, m});

                // now that the recipient has a new message, make sure that they wake up on time to do something with it
                auto recipient_iterator_ = simulation.wake_up_times.find(m->recipient);
                if(simulation.wake_up_times.end() == recipient_iterator_){
                    simulation.wake_up_times[m->recipient] = m->received;
                }else{
                    (*recipient_iterator_).second = std::min((*recipient_iterator_).second, m->received);
                }

                ++messages_;
            }

            a->outbox.clear();          // clear the outbox, so that messages are only sent once
            a->outbox.shrink_to_fit();  // this is aggressively releasing the capacity, so that periods after intense
                                        // messaging do not use too much memory
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
        auto duration_initialization_ = high_resolution_clock::now() - timer_start_run_;

        simulation::time_interval step_ = {simulation.start, simulation.end};


        const auto simulation_time_start = simulation.start;
        const auto timer_run_start_ = high_resolution_clock::now();
        auto timer_run_             = timer_run_start_;  
        std::cout << "run " << step_.lower << "/" << step_.upper << std::flush;
        constexpr double update_interval_ = 0.200; // seconds
        unsigned int width_ = 60;




        do {
            size_t changes_ = 0;
            changes_ += activate();
            changes_ += deactivate();
            step_.lower = simulation.step(step_);

            auto clock_ = high_resolution_clock::now();
            if((clock_ - timer_run_).count() / 1e+9 >= update_interval_) {
                timer_run_ = clock_;



                std::cout << "\rrun " << step_.lower << "/" << step_.upper << " [";
                auto progress_ = double(step_.upper - step_.lower) / (step_.upper - simulation_time_start);
                for(auto i = 0; i < width_; ++i) {
                    if(i < unsigned int((1.-progress_) * width_)) {
                        std::cout << '|';
                    } else {
                        std::cout << ' ';
                    }
                }
                std::cout << ']' << std::flush;
            }

        } while(step_.lower < simulation.end);
        
        std::cout << "\rrun " << step_.lower << "/" << step_.upper << " [";
        for(auto i = 0; i < width_; ++i) {
            std::cout << '|';
        }
        std::cout << ']' << std::flush;
        std::cout << std::endl;

        auto timer_simulation_ = high_resolution_clock::now() - timer_start_run_;

        LOG(notice) << "simulation took "
                    << (double(timer_simulation_.count()) / 1e+9)
                    <<  " seconds" << std::endl;

        simulation.terminate();
        auto timer_termination_ = high_resolution_clock::now() - timer_simulation_;

        after_run(simulation);
        auto timer_processing_after_ = high_resolution_clock::now() - timer_termination_;
        auto timer_total_ = high_resolution_clock::now() - timer_start_run_;

        LOG(notice) << "running simulation in " << boost::core::demangle(typeid(decltype(*this)).name())
                    << " took " << (double(timer_total_.count()) / 1e+9)
                    << " seconds" << std::endl;
    }
}// namespace esl::computation
