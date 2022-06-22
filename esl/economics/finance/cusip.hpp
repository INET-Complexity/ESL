/// \file   cusip.hpp
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
#ifndef ESL_CUSIP_HPP
#define ESL_CUSIP_HPP

#include <sstream>

#include <esl/algorithms.hpp>
#include <esl/exception.hpp>


namespace esl::economics::finance {

    ///
    /// \brief  Defines a CUSIP code as specified in the American National Standard under Accredited Standards X9.6
    ///
    struct cusip
    {
        ///
        /// \brief  A code that uniquely identifies a U.S. entity.
        ///
        std::array<char, 6> issuer;

        ///
        /// \brief  A code that is unique per security offered by the issuer.
        ///
        ///
        std::array<char, 2> code;

        ///
        /// \brief  Constructs a CUSIP from the issuer and code part
        ///
        /// \param issuer   The firm issuing the security
        /// \param code     The code part describing a specific security of the issuer,
        ///                 and "10" is the first issue by convention
        explicit constexpr cusip( const std::array<char, 6> &issuer
                                , const std::array<char, 2> &code = {'1', '0'}
                                )
        : issuer(issuer), code(code)
        {

        }

        ///
        /// \brief  Constructs a CUSIP from a complete code minus checksum.
        ///
        /// \param code     The code part describing a security
        explicit cusip(const std::array<char, 8> &code = {'0'})
            : issuer(esl::array_slice<0, 6>(code))
            , code(esl::array_slice<6, 8>(code))
        {

        }

        ///
        /// \brief  Constructs a cusip from a complete code with  checksum,
        ///         but throws if checksum doesn't match
        ///
        /// \param code     The code part describing a security
        explicit cusip(const std::array<char, 9> &code = {'0'})
            : issuer(esl::array_slice<0, 6>(code))
            , code(esl::array_slice<6, 8>(code))
        {
            if(this->checksum() != code[8]){
                throw esl::exception("invalid CUSIP checksum");
            }
        }


        ///
        /// \brief  Constructs an cusip from a complete code minus checksum.
        ///
        /// \param code     The code part describing a security
        explicit cusip(const std::string &code = "00000000")
        : cusip(esl::to_array<0, 6, char>(code), esl::to_array<6, 8, char>(code))
        {

        }

    protected:
        ///
        /// \brief Converts CUSIP symbol to numerical value
        ///
        /// \param c    symbol in the CUSIP code
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

            case '*':
                return 10 + 26 + 0;
            case '@':
                return 10 + 26 + 1;
            case '#':
                return 10 + 26 + 2;

            default:
                return 0;
            }
        }
    public:
        ///
        /// \brief  Computes CUSIP checksum and returns checksum symbol.
        ///
        /// \return checksum symbol
        [[nodiscard]] constexpr signed char checksum() const
        {
            std::uint_fast64_t sum_ = 0;

            for(uint_fast8_t i = 0; i < 8; ++i){
                auto c = i < 6 ? issuer[i] : code[i-6];
                std::uint_fast8_t value_ = value(c);
                value_ = value_ << (i % 2); // max is 38 * 2 <= 255
                sum_ += (value_ % 10) + std::uint_fast64_t(value_ / 10);
            }
            return static_cast<signed char>('0' + ((10 - sum_ % 10) % 10)); // second module is to change 10->0
        }

        ///
        /// \brief  outputs cusip code including the checksum
        ///
        /// \param stream
        /// \param i
        /// \return
        friend std::ostream &operator << (std::ostream &stream, const cusip &i)
        {
            for(auto c : i.issuer) {
                stream << c;
            }
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

#endif  // ESL_cusip_HPP
