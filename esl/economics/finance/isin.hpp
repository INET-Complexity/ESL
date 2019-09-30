/// \file   isin.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2017-10-01
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
/// Oxford Martin School, University of Oxford
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
#ifndef ESL_ISIN_HPP
#define ESL_ISIN_HPP


#include <esl/geography/country.hpp>
#include <esl/algorithms.hpp>


namespace esl::economics::finance {

    ///
    /// \brief  Defines a ISO 6166 code
    ///
    struct isin
    {
        ///
        /// \brief
        ///
        const geography::country issuer;

        ///
        /// \brief  The National Securities Identifying Number
        ///         CUSIP/SEDOL/WKN/Valor
        ///
        const std::array<char, 9> code;

        ///
        /// \brief  Constructs an ISIN from the country and the code part.
        ///
        /// \param issuer   The country issuing the code
        /// \param code     The code part describing a security
        constexpr isin(geography::country issuer,
                       const std::array<char, 9> &code = {'0'})
        : issuer(issuer), code(code)
        {

        }

        ///
        /// \brief  Constructs an ISIN from a complete code minus checksum.
        ///
        /// \param code     The code part describing a security
        constexpr isin(const std::array<char, 11> &code = {'0'})
        : issuer(geography::country({code[0], code[1]}))
        , code(esl::array_slice<0, 9>(code))
        {}

        ///
        /// \brief  Constructs an ISIN from the country and the code part.
        ///
        /// \param issuer   The country issuing the code
        /// \param code     The code part describing a security as a string with
        ///                 the code symbols in positions 0-8
        isin(geography::country issuer, const std::string &code = "000000000")
        : isin(issuer, esl::to_array<0, 9, char>(code))
        {

        }

        ///
        /// \brief  Constructs an ISIN from a complete code minus checksum.
        ///
        /// \param code     The code part describing a security
        isin(const std::string &code = "AA000000000")
        : isin(esl::to_array<0, 11, char>(code))
        {

        }

    protected:
        ///
        /// \brief Converts ISIN symbol to numerical value
        ///
        /// \param c    symbol in the ISIN code
        /// \return
        constexpr static std::uint8_t value(char c)
        {
            switch(c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return static_cast<std::uint8_t>(c - '0');
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
                return static_cast<std::uint8_t>(c - 'a') + 10;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
                return static_cast<std::uint8_t>(c - 'A') + 10;
            }
        }

        [[nodiscard]] constexpr inline static uint64_t
        difference(uint64_t number)
        {
            return number / 5 - ((number << 1u) - (number << 1u) % 10);
        }

    public:
        ///
        /// \brief  Computes ISIN checksum and returns checksum symbol.
        ///
        /// \return checksum symbol
        [[nodiscard]] constexpr signed char checksum() const
        {
            uint_fast16_t pairs_[2] = {0, 0};
            uint_fast16_t carry_[2] = {0, 0};

            uint_fast16_t p = 0u;
            uint_fast16_t i = 0u;
            uint_fast16_t n = 10u;

            const auto sequence_ = esl::array_concatenate(issuer.code, code);

            for(; i < sequence_.size(); ++i) {
                auto number_ = uint_fast16_t(value(sequence_[i]));

                auto d = number_ / n;
                auto r = number_ % n;

                if(0 != d) {
                    pairs_[++p & 0x1u] += d;
                    carry_[p   & 0x1u] += difference(d);
                }
                pairs_[++p & 0x1u] += r;
                carry_[p   & 0x1u] += difference(r);
            }

            pairs_[p & 0x1u] <<= 1u;
            pairs_[p & 0x1u] += carry_[p & 0x1u];

            auto decimal_ = (n - ((pairs_[0] + pairs_[1]) % n)) % n;

            return static_cast<signed char>('0' + decimal_);
        }

        friend std::ostream &operator<<(std::ostream &stream, const isin &i)
        {
            stream << i.issuer;
            for(auto c : i.code) {
                stream << c;
            }
            stream << i.checksum();
            return stream;
        }
    };
}  // namespace esl::economics::finance

#endif  // ESL_ISIN_HPP
