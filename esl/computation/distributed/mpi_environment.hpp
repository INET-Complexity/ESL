/// \file   parallel_environment.hpp
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
#ifndef ESL_MPI_ENVIRONMENT_HPP
#define ESL_MPI_ENVIRONMENT_HPP

#ifdef WITH_MPI

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>

#include <unordered_map>

#include <esl/agent.hpp>
#include <esl/computation/distributed/protocol.hpp>
#include <esl/computation/environment.hpp>
#include <esl/simulation/identity.hpp>

namespace esl::computation {
    struct agent_timing;
}

namespace esl::computation::distributed {


    class mpi_environment : public environment
    {
    protected:
        ///
        ///
        ///
        boost::mpi::environment environment_;

        ///
        ///
        ///
        boost::mpi::communicator communicator_;

        ///
        /// \brief  mapping from agent identity to MPI process identifier
        ///
        std::unordered_map<identity<agent>, distributed::node_identifier>
            agent_locations_;

        ///
        /// \brief  In MPI distributed mode, the communication times are
        ///         important when computational environment is not uniform.
        ///
        // std::unordered_map<identity<agent>,
        // std::unordered_map<node_identifier, timer<mean> >> communications_;
    public:
        mpi_environment();

        ~mpi_environment() override = default;


        void run(simulation::model &simulation) override;

    protected:
        ///
        /// \param simulation
        void migrate(simulation::model &simulation, agent_timing &timing);

        ///
        /// \brief  Handles agents moving between MPI processes.
        ///
        /// \param migrations
        void process_migrations(const std::vector<migration> &migrations);

        ///
        /// \brief  Decide on which agents to move away from this process to
        ///         other MPI processes.
        ///
        /// \return
        virtual std::vector<migration> migrate_agents();

        ///
        /// \brief  agents are activated when they are newly created or when
        /// they migrate from one
        ///         computational process to another
        size_t activate() override;

        ///
        /// \brief  Agents are deactivated when they are deleted, or when they
        /// migrate away from
        ///         this process
        size_t deactivate() override;

        ///
        /// \param a
        /// \param n
        void activate_agent(const identity<agent> &a) override;

        ///
        /// \param a
        /// \param node
        void activate_agent(const identity<agent> &a, node_identifier node);

        ///
        void deactivate_agent(const identity<agent> &a) override;

        void before_step() override;

        void after_step(simulation::model &simulation) override;

        ///
        /// \return
        bool is_coordinator() const;

        ///
        /// \param simulation
        /// \return
        size_t send_messages(simulation::model &simulation) override;

        void clear_agents(std::shared_ptr<simulation::model> simulation);
    };

}  // namespace esl::computation::distributed

#else  // WITH_MPI

#endif  // WITH_MPI

#endif  // ESL_MPI_ENVIRONMENT_HPP
