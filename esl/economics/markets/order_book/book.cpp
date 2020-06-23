/// \file   book.cpp
///
/// \brief
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


#include <esl/economics/markets/order_book/book.hpp>
#include <esl/data/log.hpp>


namespace esl::economics::markets::order_book {

    bool book::insert(const order &order)
    {
        if(order.side == order::buy) {
            orders_bid.insert(bid_t::value_type(order.m_price, order));
        }else if(order.side == order::sell) {
            orders_ask.insert(ask_t::value_type(order.m_price, order));
        }
        return true;
    }

    void book::erase(const order &order)
    {
        if(order.side == order::buy) {
            for(auto i = orders_bid.begin(); i != orders_bid.end(); ++i){
                if(i->second.identifier == order.identifier) {
                    orders_bid.erase(i);
                    return;
                }
            }
        } else if(order.side == order::sell) {
            for(auto i = orders_ask.begin(); i != orders_ask.end(); ++i){
                if(i->second.identifier == order.identifier) {
                    orders_ask.erase(i);
                    return;
                }
            }
        }
    }

    bool book::match(std::queue<order> &orders)
    {
        while(true) {
            if(orders_bid.empty() || orders_ask.empty()) {
                return !orders.empty();
            }

            auto i_bid_ = orders_bid.begin();
            auto i_ask_ = orders_ask.begin();

            if(i_bid_->second.m_price >= i_ask_->second.m_price) {
                order &bid = i_bid_->second;
                order &ask = i_ask_->second;

                match(bid, ask);
                orders.push(bid);
                orders.push(ask);

                if(bid.closed()) {
                    orders_bid.erase(i_bid_);
                }

                if(ask.closed()) {
                    orders_ask.erase(i_ask_);
                }
            } else {
                return !orders.empty();
            }
        }
    }

    order &book::find(order::side_t side, const esl::identity<esl::law::property>& identifier)
    {
        if(side == order::buy) {
            for(auto & i : orders_bid) {
                if (i.second.identifier == identifier) {
                    return i.second;
                }
            }
        } else if(side == order::sell) {
            for(auto & i : orders_ask) {
                if (i.second.identifier == identifier) {
                    return i.second;
                }
            }
        }

        throw std::exception();
    }

    void book::match(order &bid, order &ask)
    {
        auto price = ask.m_price;
        auto quantity_ = std::min(bid.quantity, ask.quantity);

        bid.quantity -= quantity_;
        ask.quantity -= quantity_;

        // TODO: 1 tradereport to all participants
        //       2 executionreports to buyer/seller
    }

}
