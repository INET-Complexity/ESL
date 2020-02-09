/// \file   interest_rate.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-02
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
#ifndef ESL_INTEREST_RATE_HPP
#define ESL_INTEREST_RATE_HPP

#include <esl/economics/rate.hpp>
#include <esl/mathematics/rational.hpp>

namespace esl::economics {
    ///
    ///
    ///
    struct interest_rate
    : public rate<>
    {
        esl::simulation::time_duration duration;

        ///
        /// \param rate_over_duration
        /// \param duration
        constexpr interest_rate(rational rate_over_duration = 0,
                                esl::simulation::time_duration duration = esl::simulation::time_duration(1))
        : rate<>(rate_over_duration), duration(duration)
        {

        }

        constexpr interest_rate(rate<> rate_over_duration,
                                esl::simulation::time_duration duration)
                : rate<>(rate_over_duration), duration(duration)
        {

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
                "rate_uint64_t_",
                boost::serialization::base_object<rate<>>(*this));
            archive &BOOST_SERIALIZATION_NVP(duration);
        }
    };

    struct inflation_rate
    : public interest_rate
    {
        using interest_rate::interest_rate;

        static inflation_rate from_consumer_prices(price begin, price end, esl::simulation::time_duration d)
        {
            return inflation_rate(rate<>((end - begin).value, begin.value), d );
        }
    };

    struct nominal_interest_rate
    : public interest_rate
    {
        using interest_rate::interest_rate;
    };

    struct real_interest_rate
    : public interest_rate
    {
        using interest_rate::interest_rate;
    };
}

#ifdef WITH_MPI
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::economics::interest_rate>
    : public mpl::true_
    {

    };
}
#endif

#endif  // ESL_INTEREST_RATE_HPP
