/// \file   agent.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2017-11-24
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
#include <boost/serialization/export.hpp>

#include <esl/agent.hpp>


namespace esl {

    agent::agent() : agent(identity<agent>())
    {}

    agent::agent(identity<agent> i)
    : entity<agent>(std::move(i)), communicator()
    {}

    agent::agent(const agent &o)
    : entity<agent>(o.identifier)
    , interaction::communicator(o.schedule_)
    , data::producer()
    {

    }

    simulation::time_point agent::act(simulation::time_interval step,
                                      std::seed_seq &seed)
    {
        (void) seed;
        return step.upper;
    }
}  // namespace esl

BOOST_CLASS_TRACKING(esl::agent, boost::serialization::track_always)

BOOST_CLASS_EXPORT(esl::identity<esl::agent>);

BOOST_CLASS_EXPORT(esl::agent);
