/// \file   currency.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-02-04
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
#ifndef ESL_CURRENCY_HPP
#define ESL_CURRENCY_HPP

#include <array>
#include <cassert>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>

namespace esl::economics {

    ///
    /// \brief  INSTANCE ISO 4217 Currency Code
    ///
    struct currency
    {
        const std::array<char, 3> code;

        ///
        /// \todo: rename to unit
        ///
        const std::uint64_t denominator;

        //
        constexpr explicit currency(std::array<char, 3> isocode = {'X', 'X',
                                                                   'X'},
                                    std::uint64_t denominator   = 100)
        : code(isocode), denominator(denominator)
        {
            assert('A' <= isocode[0] && 'Z' >= isocode[0]);
            assert('A' <= isocode[1] && 'Z' >= isocode[1]);
            assert('A' <= isocode[2] && 'Z' >= isocode[2]);
            assert(0 < denominator);
        }

        ~currency() = default;

        [[nodiscard]] inline constexpr bool
        operator==(const currency &operand) const
        {
            return code[0] == operand.code[0] && code[1] == operand.code[1]
                   && code[2] == operand.code[2]
                   && denominator == operand.denominator;
        }

        [[nodiscard]] inline constexpr bool
        operator!=(const currency &operand) const
        {
            return code[0] != operand.code[0] || code[1] != operand.code[1]
                   || code[2] != operand.code[2]
                   || denominator != operand.denominator;
        }


        [[nodiscard]] inline constexpr bool
        operator<(const currency &operand) const
        {
            for(auto i = 0; i < 3; ++i) {
                if(code[i] < operand.code[i]) {
                    return true;
                }
                if(code[i] > operand.code[i]) {
                    return false;
                }
            }

            return false;
        }

        [[nodiscard]] explicit operator std::string() const
        {
            return std::string(code.data(), code.size());
        }

        [[nodiscard]] std::string format(std::uint64_t quantity) const
        {
            return std::string(code.data(), code.size()) + ' '
                   + std::to_string(quantity) + '/'
                   + std::to_string(denominator);
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &boost::serialization::make_nvp(
                "code", const_cast<std::array<char, 3> &>(code));
            archive &boost::serialization::make_nvp(
                "denominator", const_cast<std::uint64_t &>(denominator));
        }

        inline friend std::ostream &operator<<(std::ostream &o,
                                               const currency &c)
        {
            o.write(c.code.data(), c.code.size());
            return o;
        }
    };

}  // namespace esl::economics

namespace std {
    template<>
    struct hash<esl::economics::currency>
    {
        constexpr size_t operator()(const esl::economics::currency &c) const
        {
            return             size_t(c.code[0] - 'A')
                        + 26 * size_t(c.code[1] - 'A')
                   + 26 * 26 * size_t(c.code[2] - 'A');
        }
    };
}  // namespace std

#ifdef WITH_MPI

#include <boost/mpi.hpp>
namespace boost ::mpi {
    template<>
    struct is_mpi_datatype<esl::economics::currency>
    : public mpl::false_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_CURRENCY_HPP
