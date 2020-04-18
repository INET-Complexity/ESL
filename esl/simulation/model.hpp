/// \file   model.hpp
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
#ifndef ESL_SIMULATION_MODEL_HPP
#define ESL_SIMULATION_MODEL_HPP

#include <memory>
#include <unordered_set>

#include <esl/simulation/time.hpp>
#include <esl/simulation/world.hpp>
#include <esl/simulation/agent_collection.hpp>
#include <esl/simulation/parameter/parametrization.hpp>

namespace esl::computation {
    class environment;
}

namespace esl::simulation {

    class model
    {
    protected:
        computation::environment &environment_;

        ///
        /// \brief  Tracks the total number of rounds that the model has run.
        ///
        unsigned int rounds_;

    public:
        ///
        /// \brief
        ///
        parameter::parametrization parameters;

        ///
        /// \brief The fixed starting point in time of the model
        ///
        const time_point start;

        ///
        /// \brief  When the simulation time reaches this point, the model
        ///         terminates.
        ///
        time_point end;

        ///
        /// \brief  The current time of the model during computation
        ///
        time_point time;

        ///
        /// \brief  Acts as the seed when using random number generators in the
        ///         model.
        ///
        const std::uint64_t sample;

        ///
        /// \brief  Simulation world, root entity.
        ///
        simulation::world world;

        ///
        /// \brief  The agents in the model.
        ///
        agent_collection agents;

        ///
        /// \brief  How often the environment should print status updates
        ///         during computation. If 0
        ///
        std::uint64_t verbosity;

        ///
        /// \brief
        ///
        /// \details    The model needs a reference to the computational
        ///             environment during construction, as the environment
        ///             needs to manage the collection of agents.
        ///
        /// \param e
        /// \param start
        /// \param end
        model( esl::computation::environment &e
             , const parameter::parametrization &parameters);

        virtual ~model() = default;

        ///
        /// \brief  All tasks that need to be done before running the model.
        ///
        virtual void initialize();

        template<typename agent_derived_t_, typename entity_type_>
        esl::identity<agent_derived_t_> create_identifier(entity_type_ &parent)
        {
            return agents.template create_identifier<agent_derived_t_>(parent);
        }


        template<typename agent_derived_t_>
        esl::identity<agent_derived_t_> create_identifier()
        {
            return agents.template create_identifier<agent_derived_t_>(world);
        }

        ///
        ///
        /// \tparam agent_derived_t_
        /// \tparam entity_type_
        /// \param parent
        /// \return
        template<typename agent_derived_t_, typename entity_type_, typename ... arguments_>
        std::shared_ptr<agent_derived_t_> create(entity_type_ &parent, arguments_ ... arguments)
        {
            return agents.template create<agent_derived_t_, entity_type_>(parent, arguments ...);
        }

        ///
        /// \brief  Create an agent globally
        ///
        /// \tparam agent_derived_t_
        /// \return
        template<typename agent_derived_t_, typename ... arguments_>
        std::shared_ptr<agent_derived_t_> create(arguments_ ... arguments)
        {
            return agents.template create<agent_derived_t_, simulation::world>(world, arguments ...);
        }

        ///
        /// \brief  Updates the model time to the next `event`.
        ///
        /// \param event    The next event or time point to simulate up to and
        ///                 including.
        /// \return         The time of the next event. Modelers are allowed to
        ///                 set result >= event.
        ///
        virtual time_point step(time_interval step);

        ///
        /// \return
        inline time_point step()
        {
            return step({time, end});
        }

        ///
        /// \brief  Run statistical analyses for the model.
        ///
        virtual void terminate();
    };
}  // namespace esl::simulation

#endif  // ESL_SIMULATION_MODEL_HPP
