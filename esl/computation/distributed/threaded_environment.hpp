/// \file   parallel_environment.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-10-17
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
#ifndef ESL_THREADED_ENVIRONMENT_HPP
#define ESL_THREADED_ENVIRONMENT_HPP

#include <unordered_map>
#include <condition_variable>

#include <esl/agent.hpp>
#include <esl/computation/distributed/protocol.hpp>
#include <esl/computation/thread_pool.hpp>
#include <esl/computation/environment.hpp>
#include <esl/simulation/identity.hpp>
#include <esl/exception.hpp>

namespace esl::computation {
    struct agent_timing;
}

namespace esl::computation::distributed {

    ///
    /// \brief
    ///
    class threaded_environment
        : public environment
    {
    protected:

        std::atomic<bool> stopped_;

        std::vector<std::thread> threads_;

        size_t rank(const std::thread::id &identifier = std::this_thread::get_id()) const
        {
            for(size_t i = 0; i < threads_.size(); ++i){
                if(threads_[i].get_id() == identifier){
                    return i;
                }
            }
            throw esl::exception("rank() of unknown thread");
        }

        std::mutex mutex_;
        std::condition_variable variable_;


        void task_()
        {
            for(size_t t = 0; !stopped_; ++t){

                std::unique_lock<std::mutex> lock_(mutex_);
                variable_.wait(lock_);
            }
        }

        ///
        /// \brief
        ///
        std::unordered_map<identity<agent>, distributed::node_identifier> agent_locations_;

    public:
        ///
        /// \brief
        ///
        explicit threaded_environment(unsigned int threads = std::thread::hardware_concurrency());

        ///
        /// \brief
        ///
        ~threaded_environment() override = default;

        ///
        /// \brief  Runs a model to termination
        ///
        /// \param simulation The model to run in this environment.
        void run(simulation::model &simulation) override;

    protected:
        ///
        /// \brief  Used to migrate agents between cluster nodes.
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
        ///         Default behaviour is to not migrate any agent.
        ///
        /// \return List of agents to move
        virtual std::vector<migration> migrate_agents();

        ///
        /// \brief  agents are activated when they are newly created or when
        /// they migrate from one
        ///         computational process to another
        size_t activate() override;

        ///
        /// \brief  Agents are deactivated when they are deleted, or when they
        ///         migrate away from this node.
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
        /// \brief  Deactives an agent, and notifies all nodes.
        /// \param a
        void deactivate_agent(const identity<agent> &a) override;

        ///
        /// \brief  Tasks to do before the simulation performs a time step
        ///
        void before_step() override;


        ///
        /// \brief  Tasks to do after the simulation performs a time step
        ///
        void after_step(simulation::model &simulation) override;

        ///
        /// \brief
        ///
        /// \return True if this node is the head node in the cluster (node
        //          results are stored).
        bool is_coordinator() const;

        ///
        /// \param simulation
        ///
        /// \return Number of messages sent
        size_t send_messages(simulation::model &simulation) override;

        void clear_agents(std::shared_ptr<simulation::model> simulation);
    };

}  // namespace esl::computation::distributed


#endif ESL_THREADED_ENVIRONMENT_HPP