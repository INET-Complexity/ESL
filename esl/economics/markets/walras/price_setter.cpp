/// \file   price_setter.cpp
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

#include <esl/economics/markets/walras/price_setter.hpp>

/*
#include <stan/services/optimize/lbfgs.hpp>
*/

#include <algorithm>
#include <numeric>


#include <esl/economics/cash.hpp>
#include <esl/economics/markets/walras/quote_message.hpp>
#include <esl/economics/markets/walras/tatonnement.hpp>
#include <esl/economics/owner.hpp>
#include <esl/simulation/time.hpp>

// template<typename floating_t_ = double, typename integer_t_ = size_t>
std::vector<int> c(const std::vector<double> &fractions, int sum)
{
    (void)sum;

    double sum_ = 0;
    int near_   = 0;
    std::vector<int> result_;
    for(const auto &f : fractions) {
        sum_ += f;
        near_ += round(f);
        result_.push_back(round(abs(f)));
    }

    int target_ = round(sum_);

    if(near_ == target_) {
        return result_;
    }


    std::vector<size_t> indices_(fractions.size());
    iota(indices_.begin(), indices_.end(), 0);

    sort(indices_.begin(), indices_.end(), [&fractions](size_t i1, size_t i2) {
        return (fractions[i1] - std::nearbyint(fractions[i1]))
               > (fractions[i2] - std::nearbyint(fractions[i2]));
    });

    for(size_t i = 0; i < (target_ - near_); ++i) {
        ++result_[indices_[i]];
    }

    return result_;
}



namespace esl::economics::markets::walras {
    price_setter::price_setter()
    : price_setter(identity<price_setter>())
    {

    }

    ///
    /// \brief
    ///
    /// \details    Initialises the differentiable variable context to 1.0 times
    /// the initial quotes. In essence, the
    ///             solver starts at 1.0 times the initial quote
    /// \param i
    /// \param traded_assets
    price_setter::price_setter( const identity<price_setter> &i
                              , law::property_map<quote> traded_properties)

    : agent(i)
    , market(i, traded_properties)
    , state(sending_quotes)
    {
        output_clearing_prices_ =
            create_output<std::vector<price>>("clearing_prices");

        this->register_callback<esl::economics::markets::walras::differentiable_order_message>(
                [this](auto msg, esl::simulation::time_interval ti, std::seed_seq &seed) {
                    (void) msg;
                    (void) seed;
                    return ti.upper;
                });
    }

    esl::simulation::time_point
    price_setter::act(esl::simulation::time_interval step, std::seed_seq &seed)
    {
        (void) seed; // TODO
        esl::simulation::time_point next_ = step.upper;
        std::vector<quote> quotes_;

        if(state == sending_quotes) {
            next_ = step.lower;
            for(const auto &[k, v] : traded_properties) {
                (void)k;
                quotes_.push_back(v);
            }
        }else{
            std::unordered_map<
                identity<agent>,
                std::shared_ptr<walras::differentiable_order_message>>
                orders_;

            for(const auto &[k, message_] : inbox) {
                (void)k;
                if(walras::differentiable_order_message::code
                   == message_->type) {
                    auto quote_ = std::dynamic_pointer_cast<
                        walras::differentiable_order_message>(message_);
                    orders_.insert({quote_->sender, quote_});
                }
            }

            if(!orders_.empty()) {
                auto scalars_ = clear_market(orders_, step);
                std::vector<price> prices_;

                for(const auto &[k, v] : traded_properties) {
                    (void)v;
                    auto price_ = cash(currencies::USD).value(scalars_[k->identifier]);
                    traded_properties.insert(std::make_pair(k, quote(price_)));  // overwrite
                    prices_.emplace_back(price_);
                    quotes_.emplace_back(quote(price_));    
                }

                output_clearing_prices_->put(step.lower, prices_);
            } else {
                for(const auto &[k, v] : traded_properties) {
                    (void)k;
                    quotes_.push_back(v);  // restore default prices
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

        for(const auto &p : participants) {
            auto m = this->template create_message<
                esl::economics::markets::walras::quote_message>(
                p, step.lower, this->identifier, p, quote_map_);
        }
        state = clearing_market;

        return next_;
    }

    ///
    /// \brief  Clear market using tatonnement. It is assumed the price_setter has received at least one excess demand
    ///         function for each property from market participants, otherwise it will return the previous price for the
    ///         property with no excess demand curve.
    ///         
    ///
    std::map<identity<law::property>, double> price_setter::clear_market
            ( const std::unordered_map<identity<agent>, std::shared_ptr<walras::differentiable_order_message>> &o
            , const esl::simulation::time_interval &step)
    {
        std::map<esl::identity<esl::law::property>, quote> quotes_;
        for(const auto &[k, v]: traded_properties) {
            (void)k;
            quotes_.insert({k->identifier, v});
        }

        tatonnement::excess_demand_model model_(quotes_);

        model_.excess_demand_functions_.clear();
        for(auto [key, function_]: o) {
            (void)key;
            model_.excess_demand_functions_.push_back(function_);
        }

        auto result_ = model_.do_compute();
        if(!result_.has_value()){
            return {};
        }

        std::map<identity<law::property>, std::tuple<quote, double>> args;

        for(const auto &[k, v] : o) {
            auto demand_ = v->excess_demand_m(args);
            auto minimum_transfer_ =
                0.0001;  // TODO: this must be deduced from the property...
            // if(fungible){
            //  minimum_transfer_ =
            //}else{
            //  minimum_transfer_ = 1
            //}

            auto property_ = traded_properties.begin();
            for(auto [k2,ed] : demand_) {
                accounting::inventory_filter<law::property> transfers_;
                auto exact_quantity_ = quantity(int(abs(ed)), 1);
                transfers_.insert(property_->first, exact_quantity_);
                property_++;
                if(ed < -minimum_transfer_) {
                    auto transferor_ = dynamic_identity_cast<law::owner<law::property>>(k);
                    auto transferee_ = reinterpret_identity_cast<law::owner<law::property>, agent>(*this);
                    auto m = this->template create_message<interaction::transfer>(
                            k, step.lower, (*this), k,
                            transferor_, transferee_, transfers_);
                } else if(ed > minimum_transfer_) {
                    auto transferor_ = reinterpret_identity_cast<law::owner<law::property>, agent>(*this);
                    auto transferee_ = dynamic_identity_cast<law::owner<law::property>>(k);
                    auto m = this->template create_message<interaction::transfer>(
                            k, step.lower, (*this), k,
                            transferor_, transferee_, transfers_);
                }
            }
        }
        return result_.value() ;
    }


}  // namespace esl::economics::markets::walras


#include <boost/serialization/export.hpp>

#include <esl/data/serialization.hpp>


BOOST_CLASS_EXPORT(std::vector<esl::economics::price>);
typedef std::tuple<esl::simulation::time_point, std::vector<esl::economics::price>> tuple_time_point_price_vector;
BOOST_CLASS_EXPORT(tuple_time_point_price_vector);
typedef std::vector<std::tuple<esl::simulation::time_point, std::vector<esl::economics::price>>> time_series_price_vector;
BOOST_CLASS_EXPORT(time_series_price_vector);
BOOST_CLASS_EXPORT(esl::data::output<std::vector<esl::economics::price>>);