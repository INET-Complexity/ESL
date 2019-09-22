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

#include <esl/agent.hpp>
#include <esl/computation/environment.hpp>
#include <esl/simulation/model.hpp>


namespace esl::computation {

    environment::environment() = default;

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
        size_t result_;
        for(auto a : deactivated_) {
            deactivate_agent(a);
            ++result_;
        }
        return result_;
    }

    ///
    /// Send and receive messages, to all nodes all at the same time
    ///
    size_t environment::send_messages(shared_ptr<simulation::model> simulation)
    {
        for(auto &[i, a] : simulation->agents.local_agents_) {
            for(auto m : a->outbox) {
                auto iterator_ =
                    simulation->agents.local_agents_.find(m->recipient);
                if(simulation->agents.local_agents_.end() == iterator_) {
                    // not in distributed mode, and no local agent matching
                    throw std::logic_error("agent not found "
                                           + m->recipient.representation());
                }

                iterator_->second->inbox.insert({m->received, m});
            }
            a->outbox.clear();
        }

        return 0;
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
        simulation::time_step step_ = {simulation.time, simulation.end};
        if(simulation.time < simulation.end) {
            simulation.step(step_);
        }

        size_t changes_ = 0;
        changes_ += activate();
        changes_ += deactivate();
    }

    ///
    /// \param simulation
    void environment::run(std::shared_ptr<simulation::model> simulation)
    {
        simulation->initialize();
        // auto next_timestep_minimum_ = simulation->time;
        simulation::time_step step_ = {simulation->start, simulation->end};
        do {
            size_t changes_ = 0;
            changes_ += activate();
            changes_ += deactivate();

            step_.lower = simulation->step(step_);
        } while(step_.lower < simulation->end);

        simulation->terminate();
    }
}  // namespace esl::computation


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