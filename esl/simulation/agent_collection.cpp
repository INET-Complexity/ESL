/// \file   agent_collection.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-27
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
#include <esl/simulation/agent_collection.hpp>

#include <esl/agent.hpp>

#include <esl/computation/environment.hpp>


namespace esl::simulation {

    agent_collection::agent_collection(computation::environment &environment_)
    : environment_(environment_)
    {

    }

    void agent_collection::activate(std::shared_ptr<agent> a)
    {
        global_agents_.insert(a->identifier);
        local_agents_.insert({a->identifier, a});
        environment_.activate_agent(a->identifier);
    }

    void agent_collection::deactivate(std::shared_ptr<agent> a)
    {
        global_agents_.erase(a->identifier);
        local_agents_.erase(a->identifier);
        environment_.deactivate_agent(a->identifier);
    }

    std::shared_ptr<agent>
        agent_collection::operator[](const identity<agent> &a)
    {
        return local_agents_[a];
    }
}
