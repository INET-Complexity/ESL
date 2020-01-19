/// \file   protocol.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-09-05
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
#ifndef ESL_PROTOCOL_HPP
#define ESL_PROTOCOL_HPP


#include <cstdint>

#include <boost/serialization/nvp.hpp>

#include <esl/simulation/identity.hpp>

namespace esl {
    class agent;
}

namespace esl::computation::distributed {

    ///
    /// \brief  Used to uniquely identify nodes in the MPI cluster.
    /// \details    defined as a signed integer
    typedef int node_identifier;


    ///
    /// \brief  Sent to notify all nodes of a new agent being created
    ///
    struct activation
    {
        node_identifier location;
        identity<agent> activated;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_NVP(location);
            archive & BOOST_SERIALIZATION_NVP(activated);
        }
    };


    ///
    /// \brief Used to notify all nodes of an agent moving from one node to
    /// another
    struct migration
    {
        node_identifier source;
        node_identifier target;
        identity<agent> migrant;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_NVP(source);
            archive & BOOST_SERIALIZATION_NVP(target);
            archive & BOOST_SERIALIZATION_NVP(migrant);
        }
    };

    ///
    /// \brief  Used to notify all nodes that an agent was deactivated
    ///
    struct deactivation
    {
        esl::identity<agent> deactivated;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_NVP(deactivated);
        }
    };
}

#ifdef WITH_MPI
#include <boost/mpi.hpp>

namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::computation::distributed::activation>
    : mpl::true_
    {};

    template<>
    struct is_mpi_datatype<esl::computation::distributed::migration>
    : mpl::true_
    {};

    template<>
    struct is_mpi_datatype<esl::computation::distributed::deactivation>
    : mpl::true_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_PROTOCOL_HPP
