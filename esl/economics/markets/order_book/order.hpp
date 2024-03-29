/// \file   order.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-07-19
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
#ifndef ESL_ECONOMICS_MARKETS_ORDER_HPP
#define ESL_ECONOMICS_MARKETS_ORDER_HPP

#include <string>
#include <utility>
#include <iomanip>
#include <ostream>

#include <esl/agent.hpp>
#include <esl/simulation/identity.hpp>
#include <esl/economics/markets/ticker.hpp>
#include <esl/economics/markets/quote.hpp>


namespace esl::economics::markets::order_book {

    ///
    ///
    /// \details:   0 is a valid quantity, as it denotes an order that was recently closed
    ///
    class limit_order
    {
    private:
    public:

        enum class lifetime_t
        {   good_until_cancelled = 0 // GTC
        ,   fill_or_kill         = 1 // FOK
        ,   immediate_or_cancel  = 2 // IOC
        } lifetime;

        enum side_t
        { buy  = 0
        , sell = 1
        } side;

        ticker symbol;

        identity<agent> owner;

        quote limit;

        std::uint32_t quantity;

        explicit limit_order ( ticker symbol = ticker()
                             , const identity<agent> &owner = identity<agent>()
                             , side_t side = buy
                             , const quote& limit = quote(price::approximate(0.))
                             , std::uint32_t quantity = 0
                             , lifetime_t lifetime = lifetime_t::good_until_cancelled
                             )
        : lifetime(lifetime)
        , side(side)
        , symbol(std::move(symbol))
        , owner(owner)
        , limit(limit)
        , quantity(quantity)
        {

        }

        [[nodiscard]] constexpr bool operator == (const limit_order &o) const
        {
            return lifetime == o.lifetime
                && side == o.side
                && symbol == o.symbol
                && owner == o.owner
                && limit == o.limit
                && quantity == o.quantity;
        }

        ///
        /// \return
        [[nodiscard]] constexpr bool closed() const
        {
            return 0 == quantity;
        }

        ///
        /// \brief
        ///
        void cancel()
        {
            quantity = 0;
        }

    protected:
        friend std::ostream &operator << (std::ostream &, const limit_order &);
    };

    inline std::ostream &operator << (std::ostream &ostream, const limit_order &order)
    {
        (void)ostream;
        (void)order;
        return ostream;
    }

}  // namespace esl::economics::markets::order_book


#endif  // ESL_ECONOMICS_MARKETS_ORDER_HPP
