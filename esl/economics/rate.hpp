/// \file   rate.cpp
///
/// \brief  A rate is a type of quote of fractional payments over some notional
///         amount
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-04
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
#ifndef ESL_RATE_HPP
#define ESL_RATE_HPP

#include <esl/economics/price.hpp>
#include <esl/mathematics/rational.hpp>
#include <esl/simulation/time.hpp>

#include <type_traits>


namespace esl::economics {

    ///
    /// \brief  Abstract base class for rates, used to add extension functions
    template<typename integer_type_ = int64_t>
    struct rate
    : public boost::rational<integer_type_>
    {
        constexpr rate(boost::rational<integer_type_> r)
        : boost::rational<integer_type_>(r)
        {

        }

        constexpr rate(integer_type_ numerator, integer_type_ denominator)
        : boost::rational<integer_type_>(numerator, (assert(denominator > 0), denominator))
        {

        }

        constexpr rate(const rate<integer_type_> &r)
        : boost::rational<integer_type_>(r)
        {

        }

        ///
        /// \brief Rate from double, truncates to zero.
        /// 
        /// \param  value_untruncated   The rate as a floating point number.
        /// \param  precision   The number of sub-units of the rate, in essence the 
        ///                     denominator in the fractional number.
        ///
        constexpr rate( double value_untruncated
                      , typename std::make_unsigned<integer_type_>::type precision = 10'000)
        : rate( static_cast<integer_type_>(value_untruncated*precision)
              , static_cast<integer_type_>(precision))
        {

        }

        virtual ~rate() = default;

        rate<integer_type_> &operator=(const rate<integer_type_> &o)
        {
            boost::rational<integer_type_>::operator=(o);
            return *this;
        }

        template<typename floating_point_t_>
        [[nodiscard]] explicit operator floating_point_t_() const
        {
            return boost::rational_cast<floating_point_t_>(*this);
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &boost::serialization::make_nvp(
                "rational_integer_type__",
                boost::serialization::base_object<
                    boost::rational<integer_type_>>(*this));
        }
    };
}  // namespace esl::economics


#ifdef WITH_MPI
namespace boost::mpi {
    template<typename integer_type_>
    struct is_mpi_datatype<esl::economics::rate<integer_type_>>
    : public mpl::true_
    {

    };
}  // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_RATE_HPP
