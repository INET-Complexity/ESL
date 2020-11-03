/// \file   book.hpp
///
/// \brief  Multiple implementations of limit order books, separated by
///         namespace.
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


#ifndef ME_BINARY_TREE_ORDER_BOOK_HPP
#define ME_BINARY_TREE_ORDER_BOOK_HPP

#include <map>
#include <queue>
#include <string>
#include <utility>

#include <esl/economics/markets/order_book/basic_book.hpp>

#include <esl/mathematics/interval.hpp>
#include <esl/data/log.hpp>


namespace esl::economics::markets::order_book {
    ///
    /// \brief  Memory-efficient, dynamically allocated order book
    ///
    /// \todo:
    ///
    class binary_tree_order_book
    : public basic_book
    {
    public:


    private:
        basic_book::order_identifier  next_;


        std::map<basic_book::order_identifier, quote> limit_orders_;
    public:
        binary_tree_order_book()
        : next_(0)
        {

        }

        typedef std::multimap< quote
                             , std::pair<basic_book::order_identifier , limit_order_message>
                             , std::greater<>
                             > bid_t;

        typedef std::multimap< quote
                             , std::pair<basic_book::order_identifier , limit_order_message>
                             , std::less<>
                             > ask_t;

        bid_t orders_bid;
        ask_t orders_ask;

        [[nodiscard]] std::optional<quote> bid() const override
        {
            if(orders_bid.empty()){
                return {};
            }
            return quote(orders_bid.begin()->first);
        }


        [[nodiscard]] std::optional<quote> ask() const override
        {
            if(orders_ask.empty()){
                return {};
            }
            return quote(orders_ask.begin()->first);
        }

        ///
        /// \brief
        ///
        /// \param order
        void insert(const limit_order_message &order) override
        {
            if(limit_order_message::side_t::buy == order.side){
                auto remainder_ = order.quantity;
                for( auto i = orders_ask.begin()
                   ; i != orders_ask.end() && order.limit >= i->first
                   ; ++ i){

                    auto executed_ = std::min(order.quantity, i->second.second.quantity);
                    remainder_ -= executed_;

                    reports.emplace_back(execution_report {
                        .state      = execution_report::match,
                        .quantity   = executed_,
                        .identifier = direct_order,
                        .side       = order.side,
                        .limit      = i->second.second.limit,
                        .owner      = order.owner
                        });

                    i->second.second.quantity -= executed_;

                    reports.emplace_back(execution_report {
                        .state      = execution_report::match,
                        .quantity   = executed_,
                        .identifier = direct_order,
                        .side       = i->second.second.side,
                        .limit      = i->second.second.limit,
                        .owner      = i->second.second.owner
                    });

                    if(0 == i->second.second.quantity){
                        orders_ask.erase(i);
                        i = orders_ask.begin();
                        limit_orders_.erase(i->second.first);
                    }
                }

                if(remainder_ > 0) {
                    // place remainder
                    auto pair_ = std::make_pair(next_, order);
                    orders_bid.emplace(order.limit, pair_);
                    limit_orders_.emplace(next_, order.limit);
                    reports.emplace_back(execution_report {
                        .state      = execution_report::placement,
                        .quantity   = order.quantity,
                        .identifier = next_,
                        .side       = order.side,
                        .limit      = order.limit,
                        .owner      = order.owner});
                    ++next_;
                }
            }else{
                auto remainder_ = order.quantity;
                for( auto i = orders_bid.begin()
                    ; i != orders_bid.end() && order.limit <= i->first
                    ; ++ i){

                    auto executed_ = std::min(order.quantity, i->second.second.quantity);
                    remainder_ -= executed_;

                    reports.emplace_back(execution_report {
                        .state      = execution_report::match,
                        .quantity   = executed_,
                        .identifier = direct_order,
                        .side       = order.side,
                        .limit      = i->second.second.limit,
                        .owner      = order.owner
                    });

                    i->second.second.quantity -= executed_;

                    reports.emplace_back(execution_report {
                        .state      = execution_report::match,
                        .quantity   = executed_,
                        .identifier = direct_order,
                        .side       = i->second.second.side,
                        .limit      = i->second.second.limit,
                        .owner      = i->second.second.owner
                    });

                    if(0 == i->second.second.quantity){
                        orders_bid.erase(i);
                        i = orders_bid.begin();
                        limit_orders_.erase(i->second.first);
                    }
                }

                if(remainder_ > 0) {
                    // place remainder
                    auto pair_ = std::make_pair(next_, order);
                    orders_ask.emplace(order.limit, pair_);
                    limit_orders_.emplace(next_, order.limit);

                    reports.emplace_back(execution_report {
                        .state      = execution_report::placement,
                        .quantity   = order.quantity,
                        .identifier = next_,
                        .side       = order.side,
                        .limit      = order.limit,
                        .owner      = order.owner});
                    ++next_;
                }
            }
        }

        void cancel(order_identifier order) override
        {

        }
    };
}  // namespace esl::economics::markets::order_book

#endif  // ME_BINARY_TREE_ORDER_BOOK_HPP
