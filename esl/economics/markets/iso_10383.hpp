/// \file   iso_10383.hpp
///
/// \brief  The MIC (ISO 10383) uniquely identifies a market; specifically,
///         identify securities trading exchanges, not general markets
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-10
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking, Oxford Martin School, University of Oxford
///
///             Licensed under the Apache License, Version 2.0 (the "License");
///             you may not use this file except in compliance with the License.
///             You may obtain a copy of the License at
///
///                 http://www.apache.org/licenses/LICENSE-2.0
///
///             Unless required by applicable law or agreed to in writing, software
///             distributed under the License is distributed on an "AS IS" BASIS,
///             WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
///             See the License for the specific language governing permissions and
///             limitations under the License.
///
///             You may obtain instructions to fulfill the attribution requirements in CITATION.cff
///
#ifndef ESL_ISO_10383_HPP
#define ESL_ISO_10383_HPP

#include <array>
#include <cassert>
#include <string>

#include <esl/exception.hpp>

namespace esl::economics::markets {
    ///
    /// \brief  Defines an ISO 10383 market identifier code
    ///
    struct iso_10383
    {
        ///
        /// \brief  4 symbol alphanumeric code, A-Z | 0-9
        ///
        const std::array<char, 4> code;

        ///
        /// \brief Construct an ISO 10383 code from 4 symbols.
        ///
        /// \param code
        explicit iso_10383(const std::array<char, 4> &code = {'M', 'K', 'T',
                                                              '0'})
        : code(code)
        {
            for(auto c : code) {
                if(!('A' <= c && c <= 'Z') && !('0' <= c && c <= '9')) {
                    throw esl::exception(std::string("invalid symbol '") + c
                                         + "' in market identifier code");
                }
            }
        }

        ///
        /// \brief Construct an ISO 10383 code from a 4 symbol string.
        /// \param  code A  4 symbol alphanumeric code A-Z | 0-9
        explicit iso_10383(const std::string &code)
        : iso_10383(std::array<char, 4> {code[0], code[1], code[2], code[3]})
        {

        }

        ///
        /// \brief  render the code as a string
        /// \return
        [[nodiscard]] std::string representation() const
        {
            return (((std::string() + code[0]) + code[1]) + code[2]) + code[3];
        }

        /*C++20 constexpr*/ bool operator == (const iso_10383 &operand) const
        {
            return code == operand.code;
        }

        /*C++20 constexpr*/ bool operator != (const iso_10383 &operand) const
        {
            return code != operand.code;
        }

        /*C++20 constexpr*/ bool operator < (const iso_10383 &operand) const
        {
            return code < operand.code;
        }

        /*C++20 constexpr*/ bool operator > (const iso_10383 &operand) const
        {
            return code > operand.code;
        }

        /*C++20 constexpr*/ bool operator <= (const iso_10383 &operand) const
        {
            return code <= operand.code;
        }

        /*C++20 constexpr*/ bool operator >= (const iso_10383 &operand) const
        {
            return code >= operand.code;
        }
    };

    std::ostream &operator << (std::ostream &stream, const iso_10383 &code)
    {
        return (stream << code.representation());
    }
} // namespace esl::economics::markets

namespace std {
    template<>
    struct hash<esl::economics::markets::iso_10383>
    {
        size_t operator() (const esl::economics::markets::iso_10383 &m) const
        {
            return std::hash<std::string>{}(m.representation());
        }
    };
}  // namespace std

#endif//ESL_ISO_10383_HPP
