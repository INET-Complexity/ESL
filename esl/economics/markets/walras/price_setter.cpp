/// \file   price_setter.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-07-19
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
#include <esl/economics/markets/walras/price_setter.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <type_traits>
using std::map;

#include <esl/economics/markets/walras/quote_message.hpp>
#include <esl/economics/markets/walras/tatonnement.hpp>
using esl::law::property;
using esl::economics::finance::securities_lending_contract;

#include <esl/economics/finance/securities_lending_contract.hpp>


template<class... T>
struct always_false : std::false_type
{ };

template<>
struct always_false<> : std::true_type
{ };

namespace esl::economics::markets::walras {
    price_setter::price_setter()
        : price_setter(identity<price_setter>())
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
    price_setter::price_setter(const identity<price_setter> &i,
                               law::property_map<quote>
                               traded_properties)
     : agent(i)
    , market(i)
    , state(sending_quotes)
    , traded_properties(traded_properties)
    {

        output_clearing_prices_ =
            create_output<std::vector<price>>("clearing_prices");


        output_volumes_ =
            create_output<std::vector<uint64_t>>("volumes");


        register_callback<walras::differentiable_order_message>(
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
    simulation::time_point price_setter::act(simulation::time_interval step, std::seed_seq &seed)
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
                std::shared_ptr<walras::differentiable_order_message>>
                orders_;
            for(const auto &[k, message_]: inbox) {
                (void)k;
                if(walras::differentiable_order_message::code == message_->type) {
                    auto order_ = std::dynamic_pointer_cast<
                        walras::differentiable_order_message>(message_);

                    if(message_->sent < step.lower) {
                        next_ = step.lower;
                        break;
                    }
                    orders_.emplace(order_->sender, order_);
                }
            }

            if(!orders_.empty()) {
                // there is at least one order so we clear the market
                auto before_ = std::chrono::high_resolution_clock::now();
                auto scalars_ = clear_market(orders_, step);
                LOG(notice) << "clearing market took " << (double((std::chrono::high_resolution_clock::now()-before_).count()) / 1e+6) <<  " milliseconds" << std::endl;


                std::vector<price> prices_;
                for(auto &[k, v]: traded_properties){
                    (void)v;
                    prices_.emplace_back(std::get<price>(v.type));
                    quotes_.emplace_back(quote(v));
                }
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
        //LOG(trace) << describe() << " " << identifier << " time " << step.lower <<  " clearing prices " << quote_map_ << std::endl;

        for(const auto &p : participants) {
            auto m = this->template create_message<walras::quote_message>(
                p, step.lower + 1, identifier, p, quote_map_);
        }
        state = clearing_market;
        return next_;
    }


    ///
    /// \brief  Applies a solution of quotes to the currently stored traded_properties
    ///
    /// \param result_
    /// \param traded_properties
    /// \return
    std::map<identity<law::property>, std::tuple<quote, double>> apply_results(
        std::map<identity<law::property>, double> result_
        , law::property_map<quote> &traded_properties
    )
    {
        std::map<identity<law::property>, std::tuple<quote, double>> solution_;
        for(auto [p, q]: traded_properties) {
            double scalar_ = result_.find(p->identifier)->second;
            std::tuple<quote, double> part_ = std::make_tuple(q, scalar_);
            solution_.emplace(p->identifier, part_);

            std::visit(
                [&, result_, p = p](auto &quote) {
                    using type_ = std::decay_t<decltype(quote)>;
                    if constexpr(std::is_same_v<type_, price>) {
                        auto value_ = int64_t(quote.value
                                              * result_.find(p->identifier)->second);
                        if(0 == value_){//} && POSITIVE){
                            value_ = 1;
                        }
                        std::get<price>(traded_properties[p].type).value =value_
                            ;

                    } else if constexpr(std::is_same_v<type_, exchange_rate>) {
                        quote = exchange_rate(
                            uint64_t(quote.numerator()
                                     * result_.find(p->identifier)->second),
                            quote.denominator());
                        traded_properties[p].type = quote;
                    } else {
                        static_assert(always_false<type_>::value,
                                      "non-exhaustive handling of quote types");
                    }
                },
                q.type);
        }
        return solution_;
    }


    ///
    /// \brief  Computes the transfers from and to the market agent
    ///
    /// \param traded_properties
    /// \param volumes_
    /// \param orders_
    /// \return
    map<identity<property>, map<identity<agent>, int64_t>> compute_transfers
        ( const law::property_map<quote> &traded_properties
            , const map<identity<property>, double> &volumes_
            , const map<identity<property>, map<identity<agent>, std::tuple<double, quantity, quantity>>> &orders_
        )
    {
        map<identity<property>, map<identity<agent>, int64_t>> transfers_;
        for(const auto &[property_, data_]: traded_properties) {
            auto i = volumes_.find(property_->identifier);
            if(volumes_.end() == i) {
                continue;  // no agents expressed interest
            }
            int64_t error_ = 0;
            std::vector<std::tuple<identity<agent>, int64_t>> allocations_;
            for(auto [p, a]: orders_.find(property_->identifier)->second) {
                auto alloc_ = int64_t(std::get<0>(a));
                error_ += alloc_;
                allocations_.emplace_back(p, alloc_);
            }
            std::sort(allocations_.begin(), allocations_.end(),
                      [](const auto &a, const auto &b) -> bool {
                          return std::abs(std::get<1>(a))
                                 < std::abs(std::get<1>(b));
                      });

            if(error_ < 0){  // assigned too many
                if(allocations_.size() < uint64_t(-error_)) {
                    LOG(notice) << "clearing price beyond rounding error" << std::endl;
                }

                for(size_t ii = 0; ii < uint64_t(-error_); ++ii) {
                    std::get<1>(allocations_[ii % allocations_.size()]) += 1;
                }
            }else{
                // assert(allocations_.size() >= uint64_t(error_));
                if(allocations_.size() < uint64_t(error_)) {
                    LOG(notice) << "clearing price beyond rounding error" << std::endl;
                }

                for(size_t ii = 0; ii < uint64_t(error_); ++ii) {
                    std::get<1>(allocations_[ii % allocations_.size()]) -= 1;
                }
            }
            auto pair_ = transfers_.emplace(property_->identifier, map<identity<agent>, int64_t>());

            for(auto [p, a]: allocations_) {
                pair_.first->second.emplace(p, a);
            }
        }
        //if(transfers_.size()){
        //    LOG(trace) << transfers_ << std::endl;
        //}
        return transfers_;
    }



    ///
    /// \brief  Clear market using tatonnement. It is assumed the price_setter
    ///         has received at least one excess demand function for each
    ///         property from market participants, otherwise
    ///         it will return the previous price for the property with no
    ///         excess demand curve.
    ///
    ///
    std::map<identity<law::property>, double> price_setter::clear_market(
        const std::unordered_map<
            identity<agent>,
            std::shared_ptr<walras::differentiable_order_message>> &orders,
        const simulation::time_interval &step)
    {
        law::property_map<quote> old_quotes_ = traded_properties;

        tatonnement::excess_demand_model model_(traded_properties);
        for(auto [key, function_] : orders) {
            (void)key;
            model_.excess_demand_functions_.push_back(function_);
        }
        auto result1_ = model_.compute_clearing_quotes();

        // if finding a price failed, return previous price vector
        if(!result1_.has_value()){
            std::map<identity<law::property>, double> previous_;
            for(const auto &[k, v] : traded_properties) {
                (void)k;
                previous_.insert({k->identifier, 1.0 * double(v)});
            }
            return previous_;
        }

        auto solution_ = apply_results(result1_.value(), traded_properties);

        ////////////////////////////////////////////////////////////////////////
        map<identity<property>, double> volumes_;
        map<identity<property>, map<identity<agent>, std::tuple<double, quantity, quantity>>> orders_;

        //  this is to normalize positive demand, so that we dont create
        //  naked short positions by inventing new property
        map<identity<property>, double> scales_;

        map<identity<property>, double> existingdemand_;

        // total supply is the maximum number of properties
        map<identity<property>, double> total_supply_;


        for(const auto &[participant, order_] : orders) {
            auto demand_ = order_->excess_demand(solution_);
            for(const auto &[property_, excess_] : demand_) {
                auto i = scales_.emplace(property_, 0.).first;
                auto j = total_supply_.emplace(property_, 0.).first;
                auto h = existingdemand_.emplace(property_, 0.).first;

                if(excess_ >= -0.00001 && excess_ <= 0.00001) {
                    continue;
                }
                auto quote_ = solution_.find(property_)->second;
                auto units_ = excess_ / (double(std::get<0>(quote_)) * std::get<1>(quote_));
                auto units_with_existing_ = units_;

                auto k = order_->supply.find(property_);
                if(order_->supply.end() != k){
                    // TODO: there is the excess, and this is the reserve
                    units_with_existing_ -= double(std::get<0>(k->second));
                    units_with_existing_ -= double(std::get<1>(k->second));

                    j->second += double(std::get<0>(k->second)) - double(std::get<1>(k->second)) ;
                }

                if(units_with_existing_ > 0.) {
                    h->second += units_with_existing_;
                }

                if(units_ > 0.){
                    i->second += units_;
                }
            }
        }

        // TODO: check to make sure we use supply properly
        //std::cout << " total_supply_ = " << total_supply_ << std::endl;
        //std::cout << " existing_demand_ = " << existingdemand_ << std::endl;
        //std::cout << " demand(scale) = " << scales_ << std::endl;



        for(const auto &[participant, order_]: orders) {
            auto demand_ = order_->excess_demand(solution_);
            for(const auto &[property_, excess_]: demand_) {
                if(excess_ >= -0.00001 && excess_ <= 0.00001) {
                    continue;
                }

                auto quote_ = solution_.find(property_)->second;

                auto units_ = (excess_ ) / (double(std::get<0>(quote_)) * std::get<1>(quote_));

                auto i = volumes_.find(property_);
                if(volumes_.end() == i){
                    i = volumes_.emplace(property_, 0).first;
                    orders_.emplace( property_
                        , map<identity<agent>, std::tuple<double, quantity, quantity>>()
                    );
                }
                i->second += abs(units_);

                auto j = order_->supply.find(property_);
                if(order_->supply.end() == j){
                    orders_.find(property_)->second.emplace( participant, std::make_tuple(units_, 0, 0));

                    //LOG(trace) << participant << " demands {" << property_ << ", "
                    //           << units_ << "}" << std::endl;
                }else{
                    //LOG(trace) << participant << " demands {" << property_ << ", "
                    //           << std::setprecision(5) << units_
                    //           << "}" << std::endl;

                    orders_.find(property_)->second.emplace(participant, std::make_tuple(units_, std::get<0>(j->second), std::get<1>(j->second)));
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////
        auto transfers_ = compute_transfers(traded_properties, volumes_, orders_);
        output_volumes_->put(step.lower, { std::uint64_t(volumes_.begin()->second )} );


        //  send_: we, the market maker, send items to participant
        //  receive_: we, the market maker, receive items from participant
        map<identity<agent>, accounting::inventory_filter<law::property>> send_, receive_;

        auto usd_ = std::make_shared<cash>(currencies::USD);

        for(const auto &[property_, data_] : traded_properties) {
            // if no agents expressed interest, transfers_ might be empty
            if(transfers_.end() == transfers_.find(*property_)) {
                continue;
            }
            for(auto &[p, v] : transfers_.find(*property_)->second) {
                if(v == 0) {
                    continue;
                }

                auto i = orders.find(p)->second->supply.find(*property_);
                if(orders.find(p)->second->supply.end() == i) {
                    auto [ii,b] = orders.find(p)->second->supply.insert({property_->identifier
                                                                            , std::make_tuple(quantity(0)
                            ,quantity(0))});
                    i = ii;
                }
                uint64_t &long_  = std::get<0>(i->second).amount;
                uint64_t &short_ = std::get<1>(i->second).amount;

                if(v > 0) {  // send properties to this agent, or receive shorts
                    if(short_ > 0) {  // there was a short position
                        assert(long_ == 0);

                        uint64_t cancel_ = std::min(uint64_t(v), short_);
                        //LOG(trace) << "cancel the short position of " << p <<" by " << cancel_ << std::endl;


                        auto short_contract_ = std::make_shared<securities_lending_contract>(identifier, p, property_->identifier, quantity(1));
                        auto r = receive_.emplace(p, accounting::inventory_filter<law::property>()).first;
                        r->second.insert(short_contract_, quantity(cancel_));

                        // ???
                        // auto s = send_.emplace(p, accounting::inventory_filter<law::property>()).first;
                        //
                        auto s = receive_.emplace(p, accounting::inventory_filter<law::property>()).first;
                        auto o = old_quotes_.find(property_);
                        auto collateral_ = usd_->amount(cancel_ * double(std::get<price>(o->second.type)) / o->second.lot);

                        s->second.insert(usd_, collateral_);
                        v -= cancel_;
                    }

                    if(uint64_t(v) > 0){
                        //LOG(trace) << p << " purchased additional " << v << std::endl;
                        accounting::inventory_filter<law::property> purchased_;
                        purchased_.insert(property_, quantity(uint64_t(v)));
                        send_.emplace(p, purchased_);

                        // they should send cash for the purchase
                        auto r = receive_.emplace(p, accounting::inventory_filter<law::property>()).first;
                        r->second.insert(usd_, usd_->amount(v * double(std::get<price>(data_.type)) / data_.lot));
                    }
                }else{  // v < 0 so participant wants to sell/short

                    if(long_ > 0){

                        uint64_t cancel_ = std::min(uint64_t(-v), long_);
                        //LOG(trace) << "cancel the long position of " << p <<" by " << cancel_ << std::endl;

                        auto r = receive_.emplace( p, accounting::inventory_filter<law::property>()).first;
                        r->second.insert(property_, quantity(cancel_));

                        auto s = send_.emplace(p, accounting::inventory_filter<law::property>()).first;
                        auto proceeds_ = usd_->amount(cancel_ * double(std::get<price>(data_.type)) / data_.lot);
                        s->second.insert(usd_, proceeds_);
                        v += cancel_;
                    }

                    if(v <  0){ // is still smaller than zero
                        // extend the short position
                        auto amount_to_extend = uint64_t(-v);
                        //LOG(trace) << "extend the short position of " << p <<" by " << amount_to_extend << std::endl;

                        auto short_contract_ = std::make_shared<securities_lending_contract>(identifier, p, property_->identifier, quantity(1));
                        auto s = send_.emplace( p, accounting::inventory_filter<law::property>()).first;

                        auto sale_ = usd_->amount(amount_to_extend * double(std::get<price>(data_.type)) / data_.lot);

                        s->second.insert(usd_, sale_);
                        s->second.insert(short_contract_, quantity(amount_to_extend));
                    }
                }
            }
        }

        // apply netting
        for(auto &[p, i_r] : receive_) {
            auto offset_ = send_.find(p);
            if(send_.end() == offset_) {
                continue;
            }

            for(auto &[k, v] : i_r.items) {
                if(0 == v.amount){
                    //i_r.items.erase(k);
                    continue;
                }

                auto property_ = offset_->second.items.find(k);
                if(offset_->second.items.end() == property_) {
                    continue;
                }
                // receiving more than sending
                if(v > property_->second) {
                    auto diff_ = v - property_->second;
                    v          = diff_;
                    offset_->second.items.erase(property_);
                } else if(v < property_->second) {  // sending more than recv
                    auto diff_        = property_->second - v;
                    property_->second = diff_;
                    //i_r.items.erase(k);
                } else {  // sending and rececing same amount
                    //i_r.items.erase(k);
                    offset_->second.items.erase(property_);
                }
            }
            if(i_r.items.empty()){
                receive_.erase(p);
            }
        }


        for(auto [p, i] : send_) {
            LOG(trace) << "market sends to " << p << " items " << i
                       << std::endl;
            this->template create_message<interaction::transfer>(
                p, step.lower, identifier, p,
                reinterpret_identity_cast<law::owner<law::property>>(
                    identifier),
                reinterpret_identity_cast<law::owner<law::property>>(p), i);
        }

        for(auto [p, i]: receive_) {
            LOG(trace) << "market receives from " << p << " items " << i
                       << std::endl;
            this->template create_message<interaction::transfer>(
                p, step.lower, p, identifier,
                reinterpret_identity_cast<law::owner<law::property>>(p),
                reinterpret_identity_cast<law::owner<law::property>>(
                    identifier),
                i);
        }

        return result1_.value();
    }

}  // namespace esl::economics::markets::walras


#include <boost/serialization/export.hpp>

#include <esl/data/serialization.hpp>


BOOST_CLASS_EXPORT(std::vector<esl::economics::price>);
typedef std::tuple<esl::simulation::time_point,
    std::vector<esl::economics::price>>
    tuple_time_point_price_vector;
BOOST_CLASS_EXPORT(tuple_time_point_price_vector);
typedef std::vector<
    std::tuple<esl::simulation::time_point, std::vector<esl::economics::price>>>
    time_series_price_vector;
BOOST_CLASS_EXPORT(time_series_price_vector);
BOOST_CLASS_EXPORT(esl::data::output<std::vector<esl::economics::price>>);