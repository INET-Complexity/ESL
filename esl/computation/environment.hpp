/// \file   environment.hpp
///
/// \brief  An environment manages computing resources when running a simulation
///         model.
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
#ifndef ESL_COMPUTATION_ENVIRONMENT_HPP
#define ESL_COMPUTATION_ENVIRONMENT_HPP

#include <unordered_map>
#include <vector>

#include <esl/simulation/identity.hpp>


namespace esl {
    class agent;

    namespace simulation {
        class model;
        class agent_collection;
    }  // namespace simulation

    namespace computation {
        ///
        /// \brief  The basic computational environment computes models in a
        ///         single process.
        ///
        class environment
        {
        protected:
            // std::unordered_map<identity<agent>, timer<mean>>
            // agent_action_time_;

            ///
            /// \brief  Keeps track of agents that were newly created, so that
            ///         all accounting can be done after the current timestep.
            ///
            ///
            std::vector<identity<agent>> activated_;

            ///
            /// \brief  keeps track of agents that were recently deactivated, so
            ///         that accounting and storing outputs can be handled after
            ///         the current timestep has completed.
            ///
            std::vector<identity<agent>> deactivated_;

        public:
            environment();

            virtual ~environment() = default;

            ///
            /// \brief  Progresses the model one timestep
            ///
            /// \details    One time step of the model (depending on its design)
            ///             comprise one or more rounds of agent interactions.
            ///
            ///
            virtual void step(simulation::model &);

            ///
            /// \brief  Simulates the model until completion, meaning the
            ///         simulation time reaches the specified end or an exit
            ///         condition is met.
            ///
            virtual void run(std::shared_ptr<simulation::model>);

        protected:
            friend class esl::simulation::agent_collection;
            ///
            /// \brief  Activates all queued agents.
            ///
            /// \return number of activated agents
            virtual size_t activate();

            ///
            /// \return
            virtual size_t deactivate();

            ///
            /// \param a
            virtual void activate_agent(const identity<agent> &a);

            ///
            ///
            /// \param a
            virtual void deactivate_agent(const identity<agent> &a);

            ///
            /// \param simulation
            /// \return
            virtual size_t
            send_messages(std::shared_ptr<simulation::model> simulation);
        };
    }  // namespace computation
}  // namespace esl


#endif  // ESL_COMPUTATION_ENVIRONMENT_HPP
