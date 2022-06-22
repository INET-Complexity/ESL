/// \file   avellaneda_stoikov.hpp
///
/// \brief  Implements the Avellaneda-Stoikov market making algorithm.
///
/// \details    "High-frequency trading in a limit order book", Marco Avellaneda and Sasha Stoikov,
///             Quantitative Finance, Vol. 8, No. 3, April 2008, 217–224
///             Assumptions:
///
///             - loss aversion, as the strategy is not defined for risk neutrality
///             - exponential utility
///             - poisson arrival of orders on either side of the book
///             - model for the instrument mid-price is a Brownian motion
///             - implementation requires estimate of exponential symmetric arrival times, using lambda = `A` exp(-`k` delta)
///             - does not use the order book data to strategically place orders
///
///
///
/// \authors    Maarten P. Scholl
/// \date       2020-04-05
/// \copyright  Copyright 2017-2020 The Institute for New Economic Thinking,
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
#ifndef ME_AVELLANEDA_STOIKOV_HPP
#define ME_AVELLANEDA_STOIKOV_HPP

#include <esl/economics/price.hpp>

#include <utility>
#include <esl/quantity.hpp>


namespace esl::economics::markets::order_book{
    ///
    /// \param mid          Market mid price, i.e. (bid+ask)/2
    /// \param n_long
    /// \param n_short
    /// \param theta        Time as fraction of the total trading session, 0.0 = market open, 1.0 = market close
    /// \param sigma        Brownian motion volatility estimate (Bachelier vol)
    /// \param gamma        Risk aversion parameter, $\gamma = 0$ implies risk neutrality, $\gamma > 0$ implies risk aversion. Paper uses 0.01 in example
    /// \param k            Arrival rate estimate $\lambda = A \exp(-k \delta)$, default is parameter from paper. Paper uses $k = 1.5$ (and $A = 140)$
    /// \return
    std::pair<price, price> avellaneda_stoikov( const price& mid
                                              , std::uint64_t n_long
                                              , std::uint64_t n_short
                                              , double theta
                                              , double sigma
                                              , double gamma
                                              , double k
                                              )
    {
        assert(0 <= theta && theta <= 1.0);
        assert(sigma > 0.);
        double reservation_ = double(mid) - (double(n_long) - double(n_short)) * gamma * sigma*sigma * (1. - theta);
        double spread_ = gamma * (sigma * sigma) * (1. - theta) + 2./gamma * std::log(1+gamma/k);
        auto bid_ = static_cast<std::int64_t>(std::floor( (reservation_ - spread_/2) * mid.valuation.denominator));
        auto ask_ = static_cast<std::int64_t>(std::ceil( (reservation_ + spread_/2) * mid.valuation.denominator));

        // when quote spread is zero and reservation price is integer.
        // possible resolution: do not introduce bias, move both ask and bid
        if(bid_ == ask_){
            ask_ += 1;
            bid_ -= 1;
        }
        assert(bid_ < ask_);

        return {price(bid_, mid.valuation), price(ask_, mid.valuation)};
    }
};


#endif //ME_AVELLANEDA_STOIKOV_HPP
