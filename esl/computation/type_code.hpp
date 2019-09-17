/// \file   type_code.hpp
///
/// \brief  Class used to mark types at compile time with a unique `code`.
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-02
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
#ifndef ESL_TYPE_CODE_HPP
#define ESL_TYPE_CODE_HPP

#include <cstdint>

namespace esl {

    ///
    /// \brief  Marks a type with an integer code, which is forced to be unique
    ///         among all classes in the build.
    ///
    /// \tparam code_integer_
    template<std::uint64_t code_integer_>
    struct type_code
    {
        static const std::uint64_t code = code_integer_;

        ///
        /// \brief  Vestigial serialization, so that inheriting classes can
        ///         call serialization on this base class.
        ///
        /// \tparam archive_t
        /// \param archive
        /// \param version
        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version) const
        {
            (void)archive;
            (void)version;
        }
    };

}  // namespace esl

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    /// \brief MPI data type, as the structure has no data
    /// \tparam type_integer_
    template<std::uint64_t type_integer_>
    struct is_mpi_datatype<esl::type_code<type_integer_>>
    : public mpl::true_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_TYPE_CODE_HPP
