/// \file   output_base.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-26
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
#ifndef ESL_DATA_OUTPUT_BASE_HPP
#define ESL_DATA_OUTPUT_BASE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#include <esl/data/representation.hpp>
#include <esl/data/stream.hpp>


namespace esl::data {
    ///
    /// \addtogroup data
    ///
    /// \brief  base class for outputs
    ///
    struct output_base
    {
        friend class boost::serialization::access;

        ///
        /// \brief The unique name for the output
        ///
        std::string name;

        ///
        /// \brief  The output is ultimately saved to one or more ostreams.
        /// TODO    If the output_base is moved between compute nodes, the state
        ///         of these streams is guaranteed to be maintained.
        ///
        /// \details
        ///
        std::vector<std::shared_ptr<stream>> streams;

        ///
        /// \brief  Whether the output values are to be buffered until the end
        ///         of the simulation or whether they should be written immediately
        ///
        bool buffered;

        ///
        /// \param value    The variable to write to the output streams.
        template<typename output_t_>
        void write(output_t_ value)
        {
            for(auto &s: streams){
                (*s) << value << std::endl;
            }
        }

        ///
        /// \brief
        ///
        /// \param name
        explicit output_base( const std::string &name = "output_base"
                            , bool buffered = true);

        ///
        /// \brief
        ///
        /// \param name
        output_base( const std::string &name , std::shared_ptr<stream> out);

        ///
        /// \param name
        /// \param streams  A collection of streams that this output has
        ///                 exclusive access to.
        output_base( const std::string &name
                            , std::vector<std::shared_ptr<stream>> streams);

        ///
        /// \brief  Recovering the output values and storage is handled by the
        /// computational environment class.
        ///
        virtual ~output_base() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_NVP(name);
        }
    };
}

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::data::output_base>
    : public mpl::false_
    {
    };
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_DATA_OUTPUT_BASE_HPP
