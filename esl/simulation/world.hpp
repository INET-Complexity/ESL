/// \file   world.hpp
///
/// \brief  The world is the root entity of a model.
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-22
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
#ifndef ESL_WORLD_HPP
#define ESL_WORLD_HPP

#include <esl/simulation/entity.hpp>

namespace esl::simulation {

    ///
    /// \brief  The root entity for a model.
    ///
    class world
    : public entity<world>
    {
    public:
        ///
        /// \brief The identity of the world is "".
        ///
        world();

        virtual ~world() = default;

        world &operator = (const world &) = delete;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;

            archive &boost::serialization::make_nvp(
                "entity⟨world⟩",
                boost::serialization::base_object<entity<world>>(*this));
        }
    };
}


#ifdef WITH_MPI
#include <boost/mpi.hpp>

namespace boost::mpi {
    ///
    /// \brief
    template<>
    struct is_mpi_datatype<esl::simulation::world>
    : is_mpi_datatype<esl::entity<esl::simulation::world>>::type
    {};
}  // namespace boost::mpi

#endif  // WITH_MPI

#endif  // ESL_WORLD_HPP
