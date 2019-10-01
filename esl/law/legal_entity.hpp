/// \file   legal_entity.hpp
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
#ifndef ESL_LEGAL_ENTITY_HPP
#define ESL_LEGAL_ENTITY_HPP

#include <array>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/serialization/nvp.hpp>

namespace esl::law {

    ///
    /// \brief  legal_entity implements the Legal Entity Identifier system
    /// described in ISO 17442.
    ///
    struct legal_entity
    {
        ///
        /// \brief The local part describing the local issuing party of the LEI.
        ///
        const std::array<char, 4> local;

        ///
        /// \brief  The code part describing uniquely a firm that is in the
        /// issuing party's business area.
        ///
        const std::array<char, 12> code;

        ///
        /// \brief  Constructs a LEI from the local part and the code part.
        /// \param local    The local part describing the local issuing party of
        /// the LEI. \param code     The code part describing uniquely a firm
        /// that is in the issuing party's business area.
        constexpr legal_entity(const std::array<char, 4> &local = {'0'},
                               const std::array<char, 12> &code = {'0'})
        : local(local), code(code)
        {

        }

        ///
        /// \param text The LEI in string form. Assumes the string contains only
        /// the LEI (20 digits). Tests the reserved
        ///             digits (which are assumed to be '00') and tests the
        ///             checksum (compared to computed value), when assertions
        ///             are enabled.
        explicit legal_entity(const std::string &text)
        : local {text[0], text[1], text[2], text[3]}
        , code {text[6],  text[7],  text[8],  text[9],  text[10], text[11],
                text[12], text[13], text[14], text[15], text[16], text[17]}
        {
            assert(18 == text.length() || 20 == text.length());
            for(auto i = 0; i < 4; ++i) {
                assert('0' <= text[i] && '9' >= text[i]);
            }
            assert('0' == text[4] && '0' == text[5]);
            for(auto i = 6; i < 18; ++i) {
                assert(('0' <= text[i] && '9' >= text[i])
                       || ('A' <= text[i] && 'Z' >= text[i]));
            }

            if(20 == text.length()){
                auto checksum_ = this->checksum();
                assert(text[18] == std::get<0>(checksum_)
                       && text[19] == std::get<1>(checksum_));
            }
        }

        ///
        /// \brief  creates a valid (but fictional) LEI code from a hash. By
        /// default the issuing unit is set to 0000, which
        ///         as of 2019-03 is not used[1], therefore it can be assumed we
        ///         don't collide with existing legal entities. However, there
        ///         can be collisions in the space of 36^12 possible LEI's
        ///
        /// [1] https://www.leiroc.org/faq/index.htm
        /// @return Fictional LEI code based on the given hash.
        ///
        template<typename hashable_t_>
        static legal_entity create(const hashable_t_ &h,
                                   const std::array<char, 4> local = {'0', '0',
                                                                      '0', '0'})
        {

            constexpr std::array<char, 36> table_ = {
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
                'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

            auto integer_ = std::hash<hashable_t_>()(h);

            std::array<char, 12> code_ = {};

            for(size_t i = 0; i < code_.size(); ++i) {
                code_[i] = table_[integer_ % table_.size()];
                integer_ /= table_.size();
            }

            return legal_entity(local, code_);
        }

        ///
        /// \brief  Computes the two digit checksum which is specified in ISO
        ///         17442. Uses uint128_t in the computation, as
        ///         at least log2(10^4 * 10^2 * 10^(12*2) * 10^2) = 106.301699
        ///         bits are required. For this, Boost.MultiPrecision is
        ///         required (which chooses the platform-appropriate integer
        ///         model).
        ///
        /// \return The checksum digits (not integer values).
        [[nodiscard]] std::tuple<char, char> checksum() const
        {
            boost::multiprecision::uint128_t sum_ = 0;
            for(auto i = 0; i < 4; ++i) {
                sum_ *= 10;
                sum_ += local[i] - '0';
            }
            sum_ *= 100;
            for(auto i = 0; i < 12; ++i) {
                if('0' <= code[i] && '9' >= code[i]) {
                    sum_ *= 10;
                    sum_ += code[i] - '0';
                } else {
                    sum_ *= 100;
                    sum_ += code[i] - 'A' + 10;
                }
            }
            sum_ *= 100;
            const uint8_t check_ = static_cast<uint8_t>(98 - (sum_ % 97));

            return {static_cast<char>(check_ / 10 + '0'),
                    static_cast<char>(check_ % 10 + '0')};
        }

        ///
        /// \brief  Only the local part and the code part are serialised, as the
        /// reserved digits are always '00' and the
        ///         checksum can be cheaply computed when rendering.
        ///
        /// \tparam archive_t
        /// \param archive
        /// \param version
        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &boost::serialization::make_nvp(
                "local", const_cast<std::array<char, 4> &>(local));
            archive &boost::serialization::make_nvp(
                "code", const_cast<std::array<char, 12> &>(code));
        }

        ///
        /// \brief  Renders the LEI: this asserts the reserved digits are '00',
        /// and freshly computes the checksum. \param o \param le \return The
        /// modified output stream
        friend std::ostream &operator<<(std::ostream &o, const legal_entity &le)
        {
            o.write(le.local.data(), le.local.size());
            o << "00";
            o.write(le.code.data(), le.code.size());
            auto checksum_ = le.checksum();
            o << std::get<0>(checksum_) << std::get<1>(checksum_);
            return o;
        }
    };

}  // namespace esl::law

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::law::legal_entity>
    : mpl::false_
    {

    };
}      // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_LEGAL_ENTITY_HPP
