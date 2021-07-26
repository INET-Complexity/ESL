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


#include <esl/data/log.hpp>
#include <esl/economics/markets/order_book/binary_tree_order_book.hpp>


namespace esl::economics::markets::order_book {

    namespace dynamically_allocated {

/*
        bool book::insert(const limit_order_message &order)
        {
            if(order.side == limit_order_message::buy) {
                index assigned_ = next_++;
                orders_bid.insert(bid_t::value_type( order.limit
                                                      , std::make_pair(assigned_, order) ));
            } else if(order.side == limit_order_message::sell) {
                index assigned_ = next_++;
                orders_ask.insert(ask_t::value_type( order.limit
                                                      , std::make_pair(assigned_, order)));
            }

            return true;
        }

        void book::erase(index identifier)
        {
            for(auto i = orders_bid.begin_(); i != orders_bid.end(); ++i) {
                if(i->second.first == identifier) {
                    orders_bid.erase(i);
                    return;
                }
            }

            for(auto i = orders_ask.begin_(); i != orders_ask.end(); ++i) {
                if(i->second.first == identifier) {
                    orders_ask.erase(i);
                    return;
                }
            }
        }

        bool book::match(std::queue<limit_order_message> &orders)
        {
            while(true) {
                if(orders_bid.empty() || orders_ask.empty()) {
                    return !orders.empty();
                }

                auto i_bid_ = orders_bid.begin_();
                auto i_ask_ = orders_ask.begin_();

                if(i_bid_->second.second.limit >= i_ask_->second.second.limit) {
                    limit_order_message &bid = i_bid_->second.second;
                    limit_order_message &ask = i_ask_->second.second;

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

        limit_order_message &book::find(limit_order_message::side_t side,
                          //const esl::identity<esl::law::property> &
                              index identifier)
        {
            if(side == limit_order_message::buy) {
                for(auto &i : orders_bid) {
                    if(i.second.first == identifier) {
                        return i.second.second;
                    }
                }
            } else if(side == limit_order_message::sell) {
                for(auto &i : orders_ask) {
                    if(i.second.first == identifier) {
                        return i.second.second;
                    }
                }
            }

            throw std::exception();
        }

        void book::match(limit_order_message &bid, limit_order_message &ask)
        {
            auto price     = ask.limit;
            auto quantity_ = std::min(bid.quantity, ask.quantity);

            bid.quantity -= quantity_;
            ask.quantity -= quantity_;

            // TODO: 1 trade report to all participants
            //       2 executionreports to buyer/seller
        }*/
    }

}
