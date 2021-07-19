/// \file   fungibility.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-28
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
#ifndef ESL_FUNGIBILITY_HPP
#define ESL_FUNGIBILITY_HPP

#include <esl/law/property.hpp>


namespace esl::economics {

    ///
    /// \brief  An object that is fungible is interchangeable for objects of the
    /// same types.
    ///
    /// \example
    ///
    struct fungible
    {
        virtual ~fungible() = default;

        [[nodiscard]] constexpr bool is_fungible() const
        {
            return true;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)archive;
            (void)version;
        }
    };


    struct infungible
    {
        virtual ~infungible() = default;

        [[nodiscard]] constexpr bool is_fungible() const
        {
            return false;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)archive;
            (void)version;
        }
    };

}  // namespace esl::economics


#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::economics::fungible>
        : public mpl::true_
    {};

    template<>
    struct is_mpi_datatype<esl::economics::infungible>
        : public mpl::true_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_FUNGIBILITY_HPP
