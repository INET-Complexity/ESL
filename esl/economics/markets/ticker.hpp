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

#include <esl/simulation/identity.hpp>
#include <esl/law/property.hpp>

struct ticker
{
public:
    esl::identity<esl::law::property> base;
    esl::identity<esl::law::property> quote;

    ticker(const esl::identity<esl::law::property> &base,
                     const esl::identity<esl::law::property> &quote    )
    : base(base)
    , quote(quote)
    {

    }

    [[nodiscard]] constexpr bool operator == (const ticker &other) const
    {
        return base == other.base && quote == other.quote;
    }

    [[nodiscard]] constexpr bool operator != (const ticker &other) const
    {
        return base != other.base || quote != other.quote;
    }

    [[nodiscard]] constexpr bool operator < (const ticker &other) const
    {
        return base < other.base || (base == other.base && quote < other.quote);
    }

    [[nodiscard]] constexpr bool operator <= (const ticker &other) const
    {
        return (*this < other) || (*this == other);
    }

    [[nodiscard]] constexpr bool operator > (const ticker &other) const
    {
        return base > other.base || (base == other.base && quote > other.quote);
    }

    [[nodiscard]] constexpr bool operator >= (const ticker &other) const
    {
        return (*this > other) || (*this == other);
    }
};

namespace std
{
    template<>
    struct hash<ticker>
    {
        std::size_t operator () (ticker const& symbol) const noexcept
        {
            auto h1 = std::hash<esl::identity<esl::law::property>>{}(symbol.base);
            auto h2 = std::hash<esl::identity<esl::law::property>>{}(symbol.quote);

            // TODO: settle on hash combination function
            return h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
        }
    };
}


#endif //ME_TICKER_HPP
