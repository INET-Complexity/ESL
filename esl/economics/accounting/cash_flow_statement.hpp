/// \file   cash_flow_statement.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-01-27
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
#ifndef ME_CASH_FLOW_STATEMENT_HPP
#define ME_CASH_FLOW_STATEMENT_HPP



#include <algorithm>
#include <map>
#include <esl/economics/owner.hpp>


namespace esl::economics::accounting {

    ///
    ///
    ///
    typedef std::multimap<esl::simulation::time_point, esl::economics::price> mutations_t;

    typedef std::function<price(esl::simulation::time_point, const esl::economics::price &)> price_transform_t;


    struct cash_flow_statement
    {
        ///
        /// \brief  Tracks all historic income values
        ///
        mutations_t mutations;

        cash_flow_statement()
        : mutations()
        {

        }

        ///
        /// \param mutations
        /// \param start starting point
        /// \param end
        /// \return
        [[nodiscard]] price
        history(const esl::simulation::time_interval &interval, price_transform_t transform, accounting::standard s) {
            std::vector<price> values_;
            for (auto i = mutations.lower_bound(interval.lower); i != mutations.end(); ++i) {
                if (interval.contains(i->first)) {
                    values_.push_back(transform(i->first, i->second));
                    assert(i->second.valuation == s.reporting_currency);
                    /// early termination
                } else if ((!esl::simulation::time_interval::right_closed() && i->first > interval.upper)
                           || i->first == interval.upper) {
                    break;
                }
            }
            auto result_ = price::approximate(0.00, s.reporting_currency);
            for (const auto &v: values_) {
                result_ += v;
            }
            return result_;
        }

        [[nodiscard]] price history(esl::simulation::time_interval interval, accounting::standard s) {
            return history(interval, [](esl::simulation::time_point t, const esl::economics::price &p) {
                (void) t;
                return p;
            }, s);
        }

        price income(esl::simulation::time_interval interval, accounting::standard s) {
            return history(interval, [](esl::simulation::time_point t, const esl::economics::price &p) {
                (void) t;
                return price(std::max((decltype(price::value)) 0, p.value), p.valuation);
            }, s);
        }

        price expenses(esl::simulation::time_interval interval, accounting::standard s) {
            return history(interval, [](esl::simulation::time_point t, const esl::economics::price &p) {
                (void) t;
                return price(std::min((decltype(price::value)) 0, p.value), p.valuation);
            }, s);
        }

        ///
        ///
        ///
        /// \param interval
        /// \param s
        /// \return
        price revenue(esl::simulation::time_interval interval, accounting::standard s) {
            return history(interval, s);
        }

    };
}




#endif //ME_CASH_FLOW_STATEMENT_HPP
