/// \file   company.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-27
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
#include <esl/economics/finance/company.hpp>

#include <tuple>

//#include <esl/interaction/transfer.hpp>


namespace esl::economics {

    std::set<identity<finance::shareholder>>
    company::unique_shareholders() const
    {
        std::set<identity<shareholder>> result_;
        for(const auto &[holder_, ownership_] : shareholders) {
            (void)ownership_;
            result_.insert(holder_);
        }
        return result_;
    }


    std::uint64_t company::total_shares() const
    {
        std::uint64_t result_ = 0;
        for(const auto &[s, c] : shares_outstanding) {
            result_ += c;
        }
        return result_;
    }


    ///
    /// \warning: TODO advanced share rules including preference, cumulative
    /// shares etc \warning: the rounding rule is in favor of the company,
    /// fractional currency dividends are not paid out! \param
    /// unappropriated_profit \return
    std::map<finance::share, std::tuple<std::uint64_t, price>>
    company::compute_dividend_per_share(const price &unappropriated_profit)
    {
        if(double(unappropriated_profit) <= 0) {
            return {};
        }

        std::map<finance::share, std::tuple<std::uint64_t, price>> result_;

        double fraction_ = double(unappropriated_profit) / total_shares();

        for(const auto &[s, q] : shares_outstanding) {
            if(s.dividend) {
                result_.insert(make_pair(
                    s, std::make_tuple(q,
                                       cash(primary_jurisdiction.tender)
                                           .value(fraction_ * q))));
            }
        }

        return result_;
    }

    company::company()
    : company(
        identity<company>(),
        law::jurisdiction(geography::countries::US, economics::currencies::USD))
    {}


    company::company(const identity<company> &i, const law::jurisdiction &j)
    : agent(i)
    , owner<cash>(i)
    , owner<finance::stock>(i)
    , organization(i, j)
    , shareholder(i)
    , identifiable_as<company>()
    , balance_sheet(i)
    , last_announced_(0)
    , last_payment_(0)
    {}


    std::optional<finance::dividend_policy>
    company::upcoming_dividend(simulation::time_interval interval)
    {
        (void)interval;
        return {};
    }


    simulation::time_point company::act(simulation::time_interval interval,
                                        std::seed_seq &s)
    {
        (void)s;

        auto possible_policy_ = upcoming_dividend(interval);
        auto next_event_      = interval.upper;

        if(!possible_policy_.has_value()) {
            return next_event_;
        }

        auto policy_ = possible_policy_.value();

        if(interval.lower >= policy_.announcement_date) {
            if(last_announced_ < policy_.announcement_date) {
                last_announced_ = policy_.announcement_date;

                for(const auto &s : unique_shareholders()) {
                    this->template create_message<
                        finance::dividend_announcement_message>(
                        s, interval.lower, this->identifier, s, policy_);
                }
            }
        } else {
            next_event_ = std::min<simulation::time_point>(
                policy_.announcement_date, next_event_);
        }

        if(interval.lower >= policy_.payable_date) {
            if(last_payment_ < policy_.announcement_date) {
                last_payment_ = policy_.announcement_date;


                /*
                   double dps_ = 0.01;
                   double variance_ = 0.01;

                   auto precision_ = currencies::USD.denominator;

                   std::normal_distribution<double> distribution_( dps_ *
                   total_shares() * precision_ , variance_ * total_shares() *
                   precision_ );

                   auto unappropriated_profit_ =
                   cash(currencies::USD).value(distribution_(generator_));

                   auto dividends_ =
                   this->compute_dividend_per_share(unappropriated_profit_);
                   for(const auto &[holder_, ownership_] : shareholders){
                       identity<owner<cash>> ic =
                   dynamic_identity_cast<owner<cash>>(holder_);
                       //this->template create_message<transfer<cash>>(  ic,
                   unappropriated_profit_); // this fails, because the
                   constructor is called with identity<agent> auto result_ =
                   std::make_shared<transfer<cash>> ( *this , ic ,
                   inventory<cash>( cash(currencies::USD) ,
                   quantity(unappropriated_profit_.value,
                   cash(currencies::USD).denomination.denominator))
                                                );
                       result_->recipient  = ic;
                       result_->sender     = identifier;
                       result_->sent       = time.lower;
                       result_->received   = time.lower;
                       this->outbox().push_back(result_);
                   }*/
            }
        } else {
            next_event_ = std::min<simulation::time_point>(
                policy_.announcement_date, next_event_);
        }
        return next_event_;
    }

}