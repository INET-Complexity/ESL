/// \file   model.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2017-12-06
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
#include <esl/simulation/model.hpp>

#include <esl/agent.hpp>
#include <esl/computation/environment.hpp>

namespace esl::simulation {
    model::model(computation::environment &e, time_point start, time_point end)
    : environment_(e), start(start), end(end), time(start), agents(e)
    {}

    void model::initialize()
    {}

    time_point model::step(time_interval step)
    {
        environment_.before_step();

        // to be set externally
        std::uint64_t sample_ = 0;


        auto first_event_   = step.upper;
        unsigned int round_ = 0;
        do {
            first_event_   = step.upper;
            for(auto &[i, a] : agents.local_agents_) {
                // The seed is deterministic in the following variables.
                std::seed_seq seed_ {
                    std::uint64_t(std::hash<identity<agent>>()(i)),
                    std::uint64_t(step.lower), std::uint64_t(round_), sample_};

                // if(agent_done_[a]) {
                //    continue;
                //}

                // agent_action_time_[a->identifier].start();
                try {
                    first_event_ = std::min(first_event_,
                                            a->process_messages(step, seed_));

                    first_event_ = std::min(first_event_, a->act(step, seed_));

                } catch(const std::runtime_error &e) {
                    // log() << "in agent[" << a->identifier <<  "].act: " <<
                    // e.what() << endl;
                    throw e;
                } catch(const std::exception &e) {
                    // log() << "in agent[" << a->identifier <<  "].act: " <<
                    // e.what() << endl;
                    throw e;
                } catch(...) {
                    // log() << "unhandled exception in agent[" << a->identifier
                    // <<
                    // "].act" << endl;
                    throw;
                }
                // agent_done_[a] = true;
                a->inbox.clear();
                // agent_action_time_[a->identifier].stop();
            }
            environment_.send_messages(*this);
        } while(step.upper > first_event_);

        environment_.after_step(*this);

        return first_event_;
    }

    void model::terminate()
    {}

}  // namespace esl::simulation


#ifdef WITH_PYTHON
#include <boost/python.hpp>
using namespace boost::python;

using namespace esl::simulation;

BOOST_PYTHON_MODULE(model)
{
    class_<model>(
        "model",
        init<esl::computation::environment &, time_point, time_point>())
        .def_readonly("start", &model::start)
        .def_readwrite("end", &model::end)
        .def_readwrite("time", &model::time);
}


#endif