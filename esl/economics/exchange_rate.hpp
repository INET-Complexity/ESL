/// \file   rate.cpp
///
/// \brief  A rate is a type of quote, part of this type are interest rates, exchange rates etc.
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-04
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
#ifndef ESL_EXCHANGE_RATE_HPP
#define ESL_EXCHANGE_RATE_HPP

#include <cstdint>

#include <esl/economics/rate.hpp>

namespace esl::economics {
    ///
    /// \brief
    ///
    /// \note
    ///
    struct exchange_rate
    : public rate<std::uint64_t>
    {
        ///
        /// \param quote
        /// \param base
        constexpr exchange_rate(std::uint64_t quote = 1, std::uint64_t base = 1)
        : rate<std::uint64_t>(quote, base)
        {
            assert(quote > 0);
            assert(base > 0);
        }

        ///
        /// \param r
        exchange_rate(const exchange_rate &r)
        : exchange_rate(r.numerator(), r.denominator())
        {

        }


        ///
        /// \brief  Constructs an exchange rate similar to the provided exchange
        ///         rate, meaning with the same precision.
        ///
        /// \param r
         exchange_rate(double f, const exchange_rate &similar)
        : exchange_rate(std::uint64_t( f * similar.denominator()), similar.denominator())
        {

        }




        ///
        /// \param o
        /// \return
        exchange_rate &operator = (const exchange_rate &o)
        {
            rate<std::uint64_t>::operator=(o);
            return *this;
        }

        ///
        /// \param p
        /// \return
        price operator * (const price &p) const
        {
            return price(static_cast<int64_t>((p.value * numerator()) / denominator()), p.valuation);
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
                "rate_std::uint64_t_",
                boost::serialization::base_object<rate<>>(*this));
        }
    };
}


#ifdef WITH_MPI
namespace boost {
    namespace mpi {
        template<>
        struct is_mpi_datatype<esl::economics::exchange_rate>
        : public mpl::true_
        {};
    }
}
#endif//WITH_MPI





#endif//ESL_EXCHANGE_RATE_HPP
