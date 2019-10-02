/// \file   share.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-03-29
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
#ifndef ESL_SHARE_HPP
#define ESL_SHARE_HPP

#include <cstdint>

#include <boost/functional/hash.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


namespace esl::economics::finance {

    ///
    /// \brief A certificate of ownership of an organization
    ///
    struct share
    {
        friend class boost::serialization::access;

        const std::uint8_t rank  = 0;
        const std::uint8_t votes = 1;
        const bool dividend      = true;
        const float preference   = 0.0;  // minimum dividend (if available)
        const bool cumulative    = false;
        const bool redeemable    = false;
        //    bool wind_up;

        constexpr bool operator==(const share &s) const
        {
            return rank == s.rank
                && votes == s.votes
                && dividend == s.dividend
                && preference == s.preference
                && cumulative == s.cumulative
                && redeemable == s.redeemable;
        }

        constexpr bool operator<(const share &s) const
        {
            return rank < s.rank;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_NVP(rank);
            archive &BOOST_SERIALIZATION_NVP(votes);
            archive &BOOST_SERIALIZATION_NVP(dividend);
            archive &BOOST_SERIALIZATION_NVP(preference);
            archive &BOOST_SERIALIZATION_NVP(cumulative);
            archive &BOOST_SERIALIZATION_NVP(redeemable);
        }
    };
}

namespace std {
    template <>
    struct hash<esl::economics::finance::share>
    {
        size_t operator()(const esl::economics::finance::share& s) const
        {
            size_t result_ = 0;
            boost::hash_combine(result_,s.rank);
            boost::hash_combine(result_,s.votes);
            boost::hash_combine(result_,s.dividend);
            boost::hash_combine(result_,s.preference);
            boost::hash_combine(result_,s.cumulative);
            boost::hash_combine(result_,s.redeemable);
            return result_;
        }
    };
}

#endif//ESL_SHARE_HPP
