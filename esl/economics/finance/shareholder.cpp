/// \file   shareholder.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-06-10
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
#include <esl/economics/finance/shareholder.hpp>

#include <esl/economics/finance/dividend.hpp>
#include <esl/economics/finance/share.hpp>
#include <esl/simulation/identity.hpp>
#include <esl/economics/finance/company.hpp>
#include <esl/economics/markets/walras/quote_message.hpp>

#include <algorithm>
using std::min;

namespace esl::economics::finance {

    shareholder::shareholder()
        : shareholder(identity<shareholder>())
    {}

    shareholder::shareholder(const identity<shareholder> &i)
        : agent(i), owner<cash>(i), owner<stock>(i), stocks()
    {
        create_output<share_holdings>("holdings");

        this->template register_callback<dividend_announcement_message>(
            [this](std::shared_ptr<dividend_announcement_message> m,
                   simulation::time_interval step) {
                ex_dividend_dates.insert(
                    {m->policy.ex_dividend_date,
                     dynamic_identity_cast<company>(m->sender)});

                return submit_dividend_record(step);
            });

        this->template register_callback<esl::economics::markets::walras::quote_message>(
            [this](std::shared_ptr<esl::economics::markets::walras::quote_message> m,
                   simulation::time_interval step) {

                for(auto &[k, v] : m->proposed){
                    assert(std::holds_alternative<price>(v.type));
                    this->prices.insert({k, std::get<price>(v.type)});
                }

                return step.upper;
            });
    }

    simulation::time_point
    shareholder::submit_dividend_record(simulation::time_interval interval)
    {
        for(auto i = ex_dividend_dates.begin(); i != ex_dividend_dates.end();
            ++i) {
            const auto &[s, c] = *i;
            if(interval.lower <= s && interval.upper >= s) {  // reply ASAP
                if(shares.find(c) != shares.end()) {
                    auto record_ = this->create_message<dividend_record>(
                        c, 0, this->identifier, c, shares[c]);
                }
            }
        }
        return interval.upper;
    }

    std::map<identity<law::property>, esl::quantity>
    shareholder::stock_holdings() const
    {
        std::map<identity<law::property>, quantity> result_;

        for(const auto &[k, v] : shares) {
            for(const auto &[share_, amount_] : v) {
                auto key_         = std::make_tuple(k, share_);
                auto property_id_ = stocks.find(key_)->second;

                auto iterator_ = result_.find(property_id_);

                if(result_.end() == iterator_) {
                    result_.insert(
                        std::make_pair(property_id_, quantity(amount_)));
                } else {
                    iterator_->second += quantity(amount_);
                }
            }
        }

        return result_;
    }
}