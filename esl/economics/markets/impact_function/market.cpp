/// \file   market.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-11-19
/// \copyright  Copyright 2017-2021 The Institute for New Economic Thinking,
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
#include <esl/economics/markets/impact_function/market.hpp>

#include <esl/economics/markets/impact_function/order.hpp>
#include <esl/economics/markets/impact_function/quote.hpp>

namespace esl::economics::markets::impact_function {
    market::market()
    : market(identity<market>())
    {

    }

    ///
    /// \brief
    ///
    /// \details    Initialises the differentiable variable context to 1.0 times
    ///             the initial quotes. In essence, the
    ///             solver starts at 1.0 times the initial quote
    /// \param i
    /// \param traded_assets
    market::market  ( const identity<market> &i
                    , law::property_map<quote> traded_properties
    )
    : agent(i)
    , esl::economics::market(i)
    , state(sending_quotes)
    , traded_properties(traded_properties)
    {
        output_clearing_prices_ = create_output<std::vector<price>>("clearing_prices");
        output_volumes_ = create_output<std::vector<uint64_t>>("volumes");

        register_callback<esl::economics::markets::impact_function::order_message>(
            [this](auto msg, simulation::time_interval ti,
                   std::seed_seq &seed) {
                (void)msg;
                (void)seed;
                return ti.upper;
            });
    }

    ///
    /// \brief
    ///
    /// \param step
    /// \param seed
    /// \return
    simulation::time_point market::act(simulation::time_interval step, std::seed_seq &seed)
    {
        (void)seed;
        simulation::time_point next_ = step.upper;
        std::vector<quote> quotes_;

        if(state == sending_quotes){
            // send out initial quotes and wait for orders,
            // scheduled in the same time step
            next_ = step.lower;
            for(const auto &[k, v]: traded_properties) {
                (void)k;
                quotes_.push_back(v);
            }
        }else{
            std::unordered_map<
                identity<agent>,
                std::shared_ptr<esl::economics::markets::impact_function::order_message>>
                orders_;
            for(const auto &[k, message_]: inbox) {
                (void)k;
                if(esl::economics::markets::impact_function::order_message::code == message_->type) {
                    auto order_ = std::dynamic_pointer_cast<esl::economics::markets::impact_function::order_message>(message_);

                    if(message_->sent < step.lower) {
                        next_ = step.lower;
                        break;
                    }
                    orders_.emplace(order_->sender, order_);
                }
            }

            if(!orders_.empty()) {
                // there is at least one order so we clear the market
                //auto before_ = std::chrono::high_resolution_clock::now();


                auto scalars_ = clear_market(orders_, step);
//                LOG(notice) << "clearing market took " << (double((std::chrono::high_resolution_clock::now()-before_).count()) / 1e+6) <<  " milliseconds" << std::endl;

                std::vector<price> prices_;
                for(auto &[k, v]: traded_properties){
                    (void)v;
                    // TODO: generalise to all types of quotes
                    prices_.emplace_back(std::get<price>(v.type));
                    quotes_.emplace_back(quote(v));
                }

//                LOG(notice) << "quotes: " << quotes_ << std::endl;

                output_clearing_prices_->put(step.lower, prices_);


            }else{  // restore previous prices
                for(const auto &[k, v]: traded_properties){
                    (void)k;
                    quotes_.push_back(v);
                }
                if(step.lower > 1){
                    next_ = step.lower;
                }
            }
        }

        law::property_map<quote> quote_map_;
        {
            size_t sequence_ = 0;
            for(const auto &[k, v]: traded_properties) {
                quote_map_.insert({k, quotes_[sequence_]});
                ++sequence_;
            }
        }
//        LOG(trace) << describe() << " " << identifier << " time " << step.lower <<  " clearing prices " << quote_map_ << std::endl;

        for(const auto &p : participants) {
            auto m = this->template create_message<impact_function::quote_message>(
                p, step.lower + 1, identifier, p, quote_map_);
        }
        state = clearing_market;
        return next_;
    }

    std::map<identity<law::property>, double> market::clear_market
        ( const std::unordered_map< identity<agent> , std::shared_ptr<order_message>> &orders
        , const esl::simulation::time_interval &step
        )
    {
        (void)step;
        law::property_map<double> volumes_;

        for(const auto &[a, m]: orders){
            for(const auto &[p, i]: m->volumes){
                volumes_.emplace(p, 0.);


                if(order_message::buy == i.first ){
                    volumes_[p] += double(i.second);
                }else if(order_message::sell == i.first ){
                    volumes_[p] -= double(i.second);
                }
            }
        }

        std::map<identity<law::property>, double> impacts_;
        for(const auto &[p, v]: volumes_){
            auto old_ = double(traded_properties[p]);
            impacts_.emplace(p->identifier, (old_ + impact_function(v)) / old_  );
        }

        return impacts_;
    }



}

