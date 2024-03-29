/// \file   dividend.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-03-19
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
#include <esl/economics/finance/dividend.hpp>

namespace esl::economics::finance {

    dividend_policy::dividend_policy(
        simulation::time_point announcement_date,
        simulation::time_point ex_dividend_date,
        const simulation::time_interval &dividend_period,
        simulation::time_point payable_date,
        const iso_4217 &dividend_currency,
        const std::map<share_class, std::tuple<std::uint64_t, price>>
        &dividend_per_share)
        : announcement_date(announcement_date)
        , ex_dividend_date(ex_dividend_date)
        , dividend_period(dividend_period)
        , payable_date(payable_date)
        , dividend_currency(dividend_currency)
        , dividend_per_share(dividend_per_share)
    {

    }

    price dividend_policy::total_dividends() const
    {
        price total_ = price::approximate(0.00, dividend_currency);
        for(const auto &[s, dividend_distribution] : dividend_per_share) {
            (void)s;
            total_ += (std::get<0>(dividend_distribution)
                       * std::get<1>(dividend_distribution));
        }
        return total_;
    }
}  // namespace esl::economics::finance

