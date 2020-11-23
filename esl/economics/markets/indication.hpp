/// \file   indication.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-08
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
#ifndef ESL_INDICATION_HPP
#define ESL_INDICATION_HPP

#include <cstdint>


namespace esl::economics::markets {
    ///
    /// \brief  Denotes whether or not a quote that is sent is binding for
    ///         the sender, meaning that the sender must deliver at the quote
    ///         if the recipient accepts the offer.
    ///
    enum indication
    : std::uint8_t
    {
        firm,
        indicative
    };

}  // namespace esl::economics::markets

#ifdef WITH_MPI
#include <boost/mpi/datatype.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::economics::markets::indication>
    : public mpl::true_
    {

    };
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_INDICATION_HPP
