/// \file   country.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-27
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
#ifndef ESL_COUNTRY_HPP
#define ESL_COUNTRY_HPP

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INSTANCE ISO 3166-1 alpha-2 Country Code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <array>
#include <iostream>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


namespace esl::geography {

    struct country
    {
        ///
        /// \brief  ISO_3166-1_alpha-2
        ///
        const std::array<char, 2> code;

        ///
        /// \param code
        ///
        explicit constexpr country(std::array<char, 2> code = {'X',
                                                               'X'}) noexcept
        : code(code)
        {

        }

        ~country() = default;

        ///
        ///
        ///
        friend std::ostream &operator<<(std::ostream &o, const country &c)
        {
            o.write(c.code.data(), c.code.size());
            return o;
        }

        ///
        /// \tparam archive_t
        /// \param archive
        /// \param version
        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &boost::serialization::make_nvp(
                "code", const_cast<std::array<char, 2> &>(code));
        }
    };
}  // namespace esl::geography


namespace std {
    template<>
    struct hash<esl::geography::country>
    {
        constexpr size_t operator()(const esl::geography::country &c) const
        {
            return size_t(c.code[0] - 'A') + 26 * size_t(c.code[1] - 'A');
        }
    };
}  // namespace std


#ifdef WITH_MPI
#include <boost/mpi.hpp>

namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::geography::country>
    : public mpl::true_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_COUNTRY_HPP
