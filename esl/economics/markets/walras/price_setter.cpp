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
#include <type_traits>
#include <map>
using std::map;

#include <esl/economics/markets/walras/quote_message.hpp>
#include <esl/economics/markets/walras/tatonnement.hpp>
using esl::law::property;

#include <esl/economics/finance/securities_lending_contract.hpp>



template <class... T>
struct always_false
: std::false_type
{

};

template <> struct always_false<>
: std::true_type
{

};

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
                               law::property_map<quote> traded_properties)

    : agent(i)
    , market(i, traded_properties)
    , state(sending_quotes)
    {
        output_clearing_prices_ =
            create_output<std::vector<price>>("clearing_prices");

        this->register_callback<walras::differentiable_order_message>(
            [this](auto msg, simulation::time_interval ti,
                   std::seed_seq &seed) {
                (void)msg;
                (void)seed;
                return ti.upper;
            });
    }

    simulation::time_point
    price_setter::act(simulation::time_interval step, std::seed_seq &seed)
    {
        (void)seed;
        simulation::time_point next_ = step.upper;
        std::vector<quote> quotes_;


        if(state == sending_quotes) {
            // send out initial quotes and wait for orders,
            // scheduled in the same time step
            next_ = step.lower;
            for(const auto &[k, v] : traded_properties) {
                (void)k;
                quotes_.push_back(v);
            }
        }else{
            std::unordered_map< identity<agent>,
                                std::shared_ptr<walras::differentiable_order_message>>
                orders_;
            for(const auto &[k, message_]: inbox) {
                (void)k;
                if(walras::differentiable_order_message::code == message_->type) {
                    auto order_ = std::dynamic_pointer_cast<
                        walras::differentiable_order_message>(message_);


                    if(message_->sent < step.lower){
                        next_ = step.lower;
                        break;
                    }

                    orders_.insert({order_->sender, order_});


                }
            }

            if(!orders_.empty()) {
                // there is at least one order so we clear the market
                auto scalars_ = clear_market(orders_, step);

                std::vector<price> prices_;
                for(auto &[k, v]: traded_properties){
                    (void)v;
                    //auto price_ =
                    //    cash(currencies::USD).value(scalars_[k->identifier]);
                    //traded_properties.insert(
                    //    std::make_pair(k, quote(price_)));  // overwrite
                    prices_.emplace_back(std::get<price>(v.type));
                    quotes_.emplace_back(quote(v));
                }
                output_clearing_prices_->put(step.lower, prices_);
                latest = step.lower;
            } else { // restore previous prices
                for(const auto &[k, v]: traded_properties) {
                    (void)k;
                    quotes_.push_back(v);
                }
            }
        }

        law::property_map<quote> quote_map_;
        {
            size_t sequence_ = 0;
            for(const auto &[k, v] : traded_properties) {
                quote_map_.insert({k, quotes_[sequence_]});
                ++sequence_;
            }
        }

        //LOG(trace) << describe() << " " << identifier << " time " << step.lower <<  " clearing prices " << quote_map_ << std::endl;

        for(const auto &p : participants) {
            auto m = this->template create_message<walras::quote_message>(
                p, step.lower + 1, this->identifier, p, quote_map_);
        }
        state = clearing_market;

        return next_;
    }

    ///
    /// \brief  Clear market using tatonnement. It is assumed the price_setter
    ///         has received at least one excess demand function for each
    ///         property from market participants, otherwise
    ///         it will return the previous price for the property with no
    ///         excess demand curve.
    ///
    ///
    std::map<identity<law::property>, double> price_setter::clear_market(const std::unordered_map<identity<agent>, std::shared_ptr<walras::differentiable_order_message>> &orders, const simulation::time_interval &step)
    {
        law::property_map<quote> old_quotes_ = this->traded_properties;

        std::map<identity<law::property>, quote> quotes_;
        for(const auto &[k, v] : traded_properties) {
            (void)k;
            quotes_.emplace(k->identifier, v);
        }

        tatonnement::excess_demand_model model_(quotes_);
        for(auto [key, function_] : orders) {
            (void)key;
            model_.excess_demand_functions_.push_back(function_);
        }
        auto result1_ = model_.compute_clearing_quotes();

        // if finding a price failed, return previous price vector
        if(!result1_.has_value()) {
            std::map<identity<law::property>, double> previous_;
            for(const auto &[k, v] : traded_properties) {
                (void)k;
                previous_.insert({k->identifier, 1.0 * double(v)});
            }
            return previous_;
        }

        auto result_ = result1_.value();



        ////////////////////////////////////////////////////////////////////////
        // round to the nearest valid price
        std::map<identity<law::property>, std::tuple<quote, double>> solution_;
        for(auto [p, q] : this->traded_properties) {
            double scalar_ = result_.find(p->identifier)->second;
            std::tuple<quote, double> part_ = std::make_tuple(q, scalar_);
            solution_.emplace(p->identifier, part_);

            std::visit([this, result_, p = p](auto &quote) {
                    using type_ = std::decay_t<decltype(quote)>;
                    if constexpr(std::is_same_v<type_, price>) {
                        std::get<price>(traded_properties[p].type).value =
                            int64_t(
                                quote.value
                                * result_.find(p->identifier)->second);

                    }else if constexpr(std::is_same_v<type_, exchange_rate>) {
                        quote =
                            exchange_rate(uint64_t(
                                quote.numerator()
                                * result_.find(p->identifier)->second), quote.denominator());
                        traded_properties[p].type = quote;
                    }else{
                         static_assert(always_false<type_>::value,
                                       "non-exhaustive handling of quotes");
                    }
                }, q.type);
        }

        ////////////////////////////////////////////////////////////////////////
        // recompute demand for solution
        ////////////////////////////////////////////////////////////////////////
        map<identity<property>, double> volumes_;
        map<identity<property>, map<identity<agent>, std::tuple<double, quantity, quantity> >> orders_;
        for(const auto &[participant, order_]: orders) {
            auto demand_ = order_->excess_demand_m(solution_);
            for(const auto &[k, v]: demand_) {
                if(v == 0) {
                    continue;
                }
                auto i = volumes_.find(k);
                if(volumes_.end() == i) {
                    i = volumes_.emplace(k, 0).first;
                    orders_.emplace(k, map<identity<agent>, std::tuple<double, quantity, quantity>>());
                }
                i->second += abs(v);

                auto j = order_->supply.find(k);
                if(order_->supply.end() == j){
                    orders_.find(k)->second.emplace(participant,
                                                    std::make_tuple(v
                                                            ,quantity(0,1)
                                                            , quantity(0,1)));
                }else{
                    orders_.find(k)->second.emplace(participant,
                                                    std::make_tuple(v
                                                            ,std::get<0>(j->second)
                                                            , std::get<1>(j->second)));
                }
            }
        }


        ////////////////////////////////////////////////////////////////////////
        // compute transfers in terms of the primary property
        map<identity<property>, map<identity<agent>, int64_t>> transfers_;
        for(const auto &[property_, data_]: traded_properties){
            auto i = volumes_.find(property_->identifier);
            if(volumes_.end() == i){
                continue;   // no agents expressed interest
            }
            int64_t accumulator_ = 0;
            std::vector<std::tuple<identity<agent>, int64_t>> allocations_;
            for(auto [p,a]: orders_.find(property_->identifier)->second){
                auto alloc_ = int64_t(std::get<0>(a));// / i->second);
                accumulator_ += alloc_;
                allocations_.push_back(std::make_tuple(p, alloc_ ));
            }
            std::sort(allocations_.begin(), allocations_.end(),
                      [](const auto &a, const auto &b) -> bool
                      {
                          return std::abs(std::get<1>(a))
                                 < std::abs(std::get<1>(b));
                      });
            //
             int64_t error_ = accumulator_;
            if(error_ < 0){ // assigned too many

                if(allocations_.size() < uint64_t(-error_)){
                    LOG(notice) << "clearing price beyond rounding error" << std::endl;
                }

                for(size_t ii = 0; ii < uint64_t(-error_); ++ii){
                    std::get<1>(allocations_[ii%allocations_.size()]) += 1;
                }
            }else{
                //assert(allocations_.size() >= uint64_t(error_));
                if(allocations_.size() < uint64_t(error_)){
                    LOG(notice) << "clearing price beyond rounding error" << std::endl;
                }

                for(size_t ii = 0; ii < uint64_t(error_); ++ii){
                    std::get<1>(allocations_[ii%allocations_.size()]) -= 1;
                }
            }
            auto pair_ = transfers_.emplace(property_->identifier, map<identity<agent>, int64_t>());

            for(auto [p,a]: allocations_) {
                 pair_.first->second.emplace(p, a);
            }
        }
        LOG(trace) << transfers_ << std::endl;

        //
        //  send_: we, the market maker, send items to participant
        //  receive_: we, the market maker, receive items from participant
        //
        map<identity<agent>,  accounting::inventory_filter<law::property> > send_, receive_;

        auto usd_ = std::make_shared<cash>(currencies::USD);


        for(const auto &[property_, data_]: traded_properties) {
            // if no agents expressed interest, transfers_ might be empty
            if(transfers_.end() == transfers_.find(*property_)){
                continue;
            }


            for(auto &[p, v]: transfers_.find(*property_)->second){
                if(v == 0){
                    continue;
                }

                auto i = orders.find(p)->second->supply.find(*property_);
                if(orders.find(p)->second->supply.end() == i){
                    continue;
                }
                uint64_t &long_amount_ = std::get<0>(i->second).amount;
                uint64_t &short_amount_ = std::get<1>(i->second).amount;

                if(v > 0){ // send properties to this agent, or receive shorts
                    if(long_amount_ > 0){ // they had long pos
                        //  purchase
                        accounting::inventory_filter<law::property> purchased_;
                        purchased_.insert(property_, quantity(uint64_t(v), 1));
                        send_.emplace(p, purchased_);
                        //  payment
                        // they should send cash for the purchase
                        auto j = receive_.find(p);
                        if(receive_.end() == j){
                            auto [jj, succes_] = receive_.emplace(p, accounting::inventory_filter<law::property>());
                            j = jj;
                        }
                        auto q__ = usd_->amount(v * double(std::get<price>(data_.type)) / data_.lot);
                        j->second.insert(usd_, q__);
                    }else if(short_amount_ > 0){// there was a short position
                        if(uint64_t(v) >= short_amount_){// cancel all
                            // 1 cancel short, by transferring shorts back to market agent
                            // cancel the short, by receiving their short contracts
                            std::map<identity<property>, esl::quantity> basket_;
                            basket_.emplace(property_->identifier, quantity(1,1));
                            auto short_contract_ = std::make_shared<finance::securities_lending_contract>(identifier, p, basket_);
                            auto r = receive_.find(p);
                            if(receive_.end() == r){
                                auto [rr, b_] = receive_.emplace(p, accounting::inventory_filter<law::property>());
                                r = rr;
                            }
                            r->second.insert(short_contract_, quantity(short_amount_,1));
                            // we pay back the collateral amount
                            auto s = send_.find(p);
                            if(send_.end() == s){
                                auto [ss, success_] = send_.emplace(p, accounting::inventory_filter<law::property>());
                                s = ss;
                            }

                            auto o = old_quotes_.find(property_);
                            auto collateral_ =
                                usd_->amount((short_amount_ * double(std::get<price>(o->second.type))) / o->second.lot);
                            s->second.insert(usd_, collateral_);

                            ////////////////////////////////////////////////////
                            //

                            v -=  short_amount_;

                            if(v > 0){
                                // this is all copy pasted from above
                                accounting::inventory_filter<law::property> purchased_;
                                purchased_.insert(property_, quantity(uint64_t(v), 1));
                                send_.emplace(p, purchased_);

                                ////////////////////////////////////////////////////////
                                //  payment
                                // they should send cash for the purchase
                                auto j = receive_.find(p);
                                if(receive_.end() == j){
                                    auto [jj, succes_] = receive_.emplace(p, accounting::inventory_filter<law::property>());
                                    j=jj;
                                }
                                j->second.insert(usd_, usd_->amount(v * double(std::get<price>(data_.type)) / data_.lot));
                            }

                        }else{// cancel in part

                            auto amount_to_cancel = v;

                            std::map<identity<property>, esl::quantity> basket_;
                            basket_.emplace(property_->identifier, quantity(1,1));
                            auto short_contract_ = std::make_shared<finance::securities_lending_contract>(identifier, p, basket_);
                            auto r = receive_.find(p);
                            if(receive_.end() == r){
                                auto [rr, b_] = receive_.emplace(p, accounting::inventory_filter<law::property>());
                                r=rr;
                            }
                            r->second.insert(short_contract_, quantity(amount_to_cancel,1));
                            // we pay back the collateral amount
                            auto s = send_.find(p);
                            if(send_.end() == s){
                                auto [ss, success_] = send_.emplace(p, accounting::inventory_filter<law::property>());
                                s=ss;
                            }

                            auto o = old_quotes_.find(property_);
                            auto collateral_ =
                                usd_->amount((amount_to_cancel * double(std::get<price>(o->second.type))) / o->second.lot);
                            s->second.insert(usd_, collateral_);
                        }
                    }
                }else{ // participant wants to sell/short///////////////////////////////////////////////////////////////////////////
                    if(long_amount_ > 0 || short_amount_ == 0) {  // they had long pos/had nothing

                        if(uint64_t(-v) >= long_amount_){// cancel all
                            // 1 cancel long, by transferring stocks back to market agent
                            auto r = receive_.find(p);
                            if(receive_.end() == r){
                                auto [rr, b_] = receive_.emplace(p, accounting::inventory_filter<law::property>());
                                r = rr;
                            }
                            r->second.insert(property_, quantity(long_amount_,1));

                            auto s = send_.find(p);
                            if(send_.end() == s){
                                auto [ss, success_] = send_.emplace(p, accounting::inventory_filter<law::property>());
                                s = ss;
                            }
                            auto proceeds_ =
                                usd_->amount((long_amount_ * double(std::get<price>(data_.type))) / data_.lot);
                            s->second.insert(usd_, proceeds_);


                            ////////////////////////////////////////////////////
                            //

                            v +=  long_amount_;
                            if(v < 0){
                                std::map<identity<property>, esl::quantity> basket_;
                                basket_.emplace(property_->identifier, quantity(1,1));
                                auto short_contract_ = std::make_shared<finance::securities_lending_contract>(identifier, p, basket_);

                                auto j = send_.find(p);
                                if(send_.end() == j){
                                    auto [jj, succes_] = send_.emplace(p, accounting::inventory_filter<law::property>());
                                    j=jj;
                                }

                                j->second.insert(short_contract_, quantity(uint64_t(-v), 1));
                                j->second.insert(usd_, usd_->amount(-v * double(std::get<price>(data_.type)) / data_.lot));
                            }

                        }else{// cancel in part

                            auto amount_to_cancel = -v;
                            auto r = receive_.find(p);
                            if(receive_.end() == r){
                                auto [rr, b_] = receive_.emplace(p, accounting::inventory_filter<law::property>());
                                r = rr;
                            }
                            r->second.insert(property_, quantity(amount_to_cancel,1));


                            // we pay back the collateral amount
                            auto s = send_.find(p);
                            if(send_.end() == s){
                                auto [ss, success_] = send_.emplace(p, accounting::inventory_filter<law::property>());
                                s = ss;
                            }
                            auto proceeds_ =
                                usd_->amount((amount_to_cancel * double(std::get<price>(data_.type))) / data_.lot);
                            s->second.insert(usd_, proceeds_);
                        }
                    }else{
                        //extend the short position
                        auto amount_to_extend = -v;

                        std::map<identity<property>, esl::quantity> basket_;
                        basket_.emplace(property_->identifier, quantity(1,1));
                        auto short_contract_ = std::make_shared<finance::securities_lending_contract>(identifier, p, basket_);
                        auto s = send_.find(p);
                        if(send_.end() == s){
                            auto [ss, b_] = receive_.emplace(p, accounting::inventory_filter<law::property>());
                            s=ss;
                        }

                        auto sale_ =
                            usd_->amount((amount_to_extend * double(std::get<price>(data_.type))) / data_.lot);
                        s->second.insert(usd_, sale_);
                        s->second.insert(short_contract_, quantity(amount_to_extend,1));
                    }
                }
            }
        }

        for(auto [p,i]: send_){
            LOG(trace) << "market sends to " << p << " items " << i << std::endl;
            this->template create_message<interaction::transfer>( p
                                                                , step.lower
                                                                , identifier
                                                                 , p
                                                                 , reinterpret_identity_cast<law::owner<law::property>>(identifier)
                                                                 , reinterpret_identity_cast<law::owner<law::property>>(p)
                                                                 , i
                                                                );
        }

        for(auto [p,i]: receive_){
            LOG(trace) << "market receives from " << p << " items " << i << std::endl;
            this->template create_message<interaction::transfer>( p
                , step.lower
                , p
                , identifier
                , reinterpret_identity_cast<law::owner<law::property>>(p)
                , reinterpret_identity_cast<law::owner<law::property>>(identifier)
                , i
            );
        }

        return result_;
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