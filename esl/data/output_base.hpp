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

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

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
        /// \brief
        ///
        /// \param name
        explicit output_base(const std::string &name = "output_base");

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
