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


#include <esl/geography/iso_3166_1_alpha_2.hpp>
#include <esl/economics/finance/cusip.hpp>
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
        geography::iso_3166_1_alpha_2 issuer;

        ///
        /// \brief  The National Securities Identifying Number
        ///         CUSIP/SEDOL/WKN/Valor
        ///
        std::array<char, 9> code;

        ///
        /// \brief  Constructs an ISIN from the country and the code part.
        ///
        /// \param issuer   The country issuing the code
        /// \param code     The code part describing a security
        constexpr isin(geography::iso_3166_1_alpha_2 issuer,
                       const std::array<char, 9> &code)
        : issuer(issuer)
        , code(code)
        {

        }

        ///
        /// \brief  Constructs an ISIN from a complete code minus checksum.
        ///
        /// \param code     The code part describing a security
        constexpr isin(const std::array<char, 11> &code)
        : issuer(  geography::iso_3166_1_alpha_2(std::array<char, 2>{code[0], code[1]})
                  //: throw esl::exception("ISO 3166-1 alpha-2 country code needs to be two characters")

                )
        , code(esl::array_slice<0, 9>(code))
        {

        }

        ///
        /// \brief  Constructs an ISIN from the country and the code part.
        ///
        /// \param issuer   The country issuing the code
        /// \param code     The code part describing a security as a string with
        ///                 the code symbols in positions 0-8
        isin(geography::iso_3166_1_alpha_2 issuer, const std::string &nsin_code)
        : isin(issuer, esl::to_array<0, 9, char>(nsin_code))
        {
            if((9 > code.size()) || (10 < code.size())){
                throw esl::exception("A valid ISIN has 9 symbols in the NSIN part, or 9 symbols plus the ISIN checksum.");
            }
        }

        ///
        /// \brief  Constructs an ISIN from the country and the code part.
        ///
        /// \param issuer   The country issuing the code
        /// \param code     The code part describing a security as a string with
        ///                 the code symbols in positions 0-8
        isin(const std::string &issuer, const std::string &nsin_code)
        : isin(geography::iso_3166_1_alpha_2(issuer), esl::to_array<0, 9, char>(nsin_code))
        {
            if(2 != issuer.size()){
                throw esl::exception("The issuer part of the ISIN must be 2 letters (ISO 3166)");
            }
        }

        ///
        /// \brief  Constructs an ISIN from a complete code, and recomputes checksum.
        ///
        /// \param code     The code part describing a security
        explicit isin(const std::string &code = "AA000000000")
        : isin(esl::to_array<0, 11, char>(code))
        {
            if(!(11 == code.size() || 12 == code.size())){
                throw esl::exception("A valid ISIN is constructed from 11 or 12 symbols.");
            }
        }

        ///
        /// \brief  Constructs an ISIN from a CUSIP code which is a United States specific code system
        ///
        /// \param code     The code part describing a security
        explicit isin(geography::iso_3166_1_alpha_2 issuer, const cusip &c)
        : isin( issuer
              , esl::array_concatenate( esl::array_concatenate(c.issuer, c.code)
                                      , std::array<char, 1>({c.checksum()})
                                      )
              )
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
            default:
                return 0;
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
                    carry_[p & 0x1u] += uint_fast16_t(difference(d));
                }
                pairs_[++p & 0x1u] += r;
                carry_[p   & 0x1u] += uint_fast16_t(difference(r));
            }

            pairs_[p & 0x1u] <<= 1u;
            pairs_[p & 0x1u] += carry_[p & 0x1u];

            auto decimal_ = (n - ((pairs_[0] + pairs_[1]) % n)) % n;

            return static_cast<signed char>('0' + decimal_);
        }

        ///
        /// \brief  outputs isin code including the checksum
        ///
        /// \param stream
        /// \param i
        /// \return
        friend std::ostream &operator << (std::ostream &stream, const isin &i)
        {
            stream << i.issuer;
            for(auto c : i.code) {
                stream << c;
            }
            stream << i.checksum();
            return stream;
        }

        [[nodiscard]] std::string representation() const
        {
            std::stringstream stream_;
            stream_ << *this;
            return stream_.str();
        }
    };
}  // namespace esl::economics::finance

#endif  // ESL_ISIN_HPP
