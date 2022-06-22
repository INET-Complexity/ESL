/// \file   ticker.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-06-08
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

#ifndef ME_TICKER_HPP
#define ME_TICKER_HPP

#include <esl/law/property.hpp>
#include <esl/simulation/identity.hpp>

namespace esl::economics::markets {

    struct ticker
    {
    public:
        ///
        /// \brief  The property we are buying
        ///
        identity<law::property> base;

        ///
        /// \brief  The property that is exchanged for it and thus sets the
        /// price
        ///
        identity<law::property> quote;

        constexpr ticker( const identity<law::property> &base  = identity<law::property>()
                        , const identity<law::property> &quote = identity<law::property>()
                        )
        : base(base)
        , quote(quote)
        { 
        
        }

        constexpr ticker(const ticker &o) 
        : base(o.base)
        , quote(o.quote)
        { 
        
        }

        [[nodiscard]] constexpr bool operator==(const ticker &other) const
        {
            return base == other.base && quote == other.quote;
        }

        [[nodiscard]] constexpr bool operator!=(const ticker &other) const
        {
            return base != other.base || quote != other.quote;
        }

        [[nodiscard]] constexpr bool operator<(const ticker &other) const
        {
            return base < other.base
                   || (base == other.base && quote < other.quote);
        }

        [[nodiscard]] constexpr bool operator<=(const ticker &other) const
        {
            return (*this < other) || (*this == other);
        }

        [[nodiscard]] constexpr bool operator>(const ticker &other) const
        {
            return base > other.base
                   || (base == other.base && quote > other.quote);
        }

        [[nodiscard]] constexpr bool operator>=(const ticker &other) const
        {
            return (*this > other) || (*this == other);
        }

 protected:
        friend std::ostream &operator<<(std::ostream &, const ticker &);
    };

    inline std::ostream &operator<<(std::ostream &ostream, const ticker &t)
    {
        (void)ostream;
        ostream << t.base << '/' << t.quote;
        return ostream;
    }
}  // namespace esl::economics::markets

namespace std {
    template<>
    struct hash<esl::economics::markets::ticker>
    {
        std::size_t
        operator()(esl::economics::markets::ticker const &symbol) const noexcept
        {
            size_t h1 = std::hash<esl::identity<esl::law::property>> {}(symbol.base);
            size_t h2 = std::hash<esl::identity<esl::law::property>> {}(symbol.quote);

            // TODO: settle on hash combination function
            return h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
        }
    };
}  // namespace std

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::economics::markets::ticker> : public mpl::false_
    { };
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ME_TICKER_HPP
