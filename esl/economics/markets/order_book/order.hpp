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

#include <iomanip>
#include <ostream>
#include <string>

#include <esl/agent.hpp>
#include <esl/simulation/identity.hpp>
#include <esl/economics/markets/ticker.hpp>
#include <esl/economics/price.hpp>


namespace esl::economics::markets::order_book {

    class order
    {
    private:
    public:

        enum lifetime_t
        {   good_until_cancelled = 0 // GTC
        ,   fill_or_kill         = 1 // FOK
        ,   immediate_or_cancel  = 2 // IOC
        } lifetime;

        enum side_t
        { buy
        , sell
        } side;

        ticker m_symbol;

        esl::identity<esl::agent> owner;

        ///
        /// \brief  An identifier provided by the owner
        ///
        esl::identity<order> identifier;

        esl::economics::price m_price;

        std::uint64_t quantity;

        order( const ticker &symbol
             , const esl::identity<esl::agent> &owner
             , const esl::identity<order> &clientId
             , side_t side
             , price limit
             , std::uint64_t quantity
             , lifetime_t lifetime = good_until_cancelled
             )
        : lifetime(lifetime)
        , side(side)
        , identifier(clientId)
        , m_symbol(symbol)
        , owner(owner)
        , m_price(limit)
        , quantity(quantity)
        {

        }

        [[nodiscard]] constexpr bool closed() const
        {
            return 0 == quantity;
        }

        void cancel()
        {
            quantity = 0;
        }

    protected:
        friend std::ostream &operator << (std::ostream &, const order &);
    };


    inline std::ostream &operator << (std::ostream &ostream, const order &order)
    {

    }

}  // namespace esl::economics::markets::order_book


#endif  // ESL_ECONOMICS_MARKETS_ORDER_HPP
