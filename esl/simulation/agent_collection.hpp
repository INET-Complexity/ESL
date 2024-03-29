/// \file   agent_collection.hpp
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
#ifndef ESL_AGENT_COLLECTION_HPP
#define ESL_AGENT_COLLECTION_HPP

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>

#include <esl/simulation/identity.hpp>


namespace esl {
    class agent;

    namespace computation {
        class environment;
    }
}  // namespace esl

namespace esl::simulation {
    template<typename entity_type_>
    struct entity;

    class agent_collection
    {
    protected:
        std::reference_wrapper<computation::environment> environment_;

        ///
        /// \brief  The identities of all agents in the simulation.
        ///
        /// \todo   Memory usage of this datastructure can be reduced by means
        ///         of a trie datastructure.
        ///
    public:
        boost::container::flat_set<identity<agent>> global_agents_;

        boost::container::flat_map<identity<agent>, std::shared_ptr<agent>> local_agents_;

        explicit agent_collection(std::reference_wrapper<computation::environment> environment_);

        template<typename agent_derived_t_, typename entity_type_>
        esl::identity<agent_derived_t_> create_identifier(entity_type_ &parent)
        {
            auto identifier_ =
                parent.identifier.template create<agent_derived_t_>(parent);
            return identifier_;
        }

        template<typename agent_derived_t_, typename entity_type_, typename ... arguments_>
        std::shared_ptr<agent_derived_t_> create(entity_type_ &parent, arguments_ ... arguments)
        {
            auto identifier_ =
                parent.identifier.template create<agent_derived_t_>(parent);
            std::shared_ptr<agent_derived_t_> result_ =
                std::make_shared<agent_derived_t_>((identifier_), arguments ...);
            // The agent is now constructed, so we can lock its callback
            // data structure.
            result_->locked_ = true;
            activate(result_);
            return result_;
        }

        void activate(std::shared_ptr<agent> a);

        void deactivate(std::shared_ptr<agent> a);

        std::shared_ptr<agent> operator[](const identity<agent> &a);
    };
}  // namespace esl::simulation

#endif  // ESL_AGENT_COLLECTION_HPP
