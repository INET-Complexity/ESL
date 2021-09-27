/// \file   sedol.hpp
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
#ifndef ESL_SEDOL_HPP
#define ESL_SEDOL_HPP

#include <sstream>

#include <esl/algorithms.hpp>
#include <esl/exception.hpp>


namespace esl::economics::finance {

    ///
    /// \brief  Defines a SEDOL code.
    ///
    struct sedol
    {
        ///
        /// \brief  6 digits, 0-9a-zA-Z
        ///
        ///
        std::array<char, 6> code;

        ///
        /// \brief  Constructs a SEDOL from the issuer and code part
        ///
        /// \param issuer   The firm issuing the security
        /// \param code     The code part describing a specific security of the issuer,
        ///                 and "10" is the first issue by convention
        explicit constexpr sedol( const std::array<char, 6> &code)
        : code(code)
        {
            for(auto c: code){
                assert(std::isdigit(c) || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'));
            }
        }


        ///
        /// \brief  Constructs an sedol from a complete code minus checksum.
        ///
        /// \param code     The code part describing a security
        explicit sedol(const std::string &code = "000000")
        : sedol(esl::to_array<0, 6, char>(code))
        {
            assert(6 == code.size() || 7 == code.size());

            if(7 == code.size()){
                assert(std::isdigit(code[6]));
            }
        }

    protected:
        ///
        /// \brief Converts sedol symbol to numerical value
        ///
        /// \param c    symbol in the sedol code
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
    public:
        ///
        /// \brief  Computes sedol checksum and returns checksum symbol.
        ///
        /// \return checksum symbol
        [[nodiscard]] constexpr char checksum() const
        {
            constexpr std::array<std::uint64_t, 6> weights_ = {1, 3, 1, 7, 3, 9};

            std::uint_fast16_t sum_ = 0;
            for(size_t i = 0; i < 6; ++i){
                sum_ += value( code[i] ) * weights_[i];
            }

            return static_cast<char>('0' + (10 - (sum_ % 10)) % 10);
        }

        ///
        /// \brief  outputs sedol code including the checksum
        ///
        /// \param stream
        /// \param i
        /// \return
        friend std::ostream &operator << (std::ostream &stream, const sedol &i)
        {
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

#endif  // ESL_SEDOL_HPP
