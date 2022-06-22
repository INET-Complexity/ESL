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
                             , std::pair<basic_book::order_identifier , limit_order>
                             , std::greater<>
                             > bid_t;

        typedef std::multimap< quote
                             , std::pair<basic_book::order_identifier , limit_order>
                             , std::less<>
                             > ask_t;

        /// 
        /// \brief  All active buy orders ordered by price descending
        /// 
        bid_t orders_bid;

        ///
        /// \brief  All active sell orders ordered by price ascending
        /// 
        ask_t orders_ask;

        ///
        /// \brief  The best bid
        /// 
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
        void insert(const limit_order &order) override
        {
            if(limit_order::side_t::buy == order.side){
                auto remainder_ = order.quantity;

                std::vector<decltype(orders_ask.begin())> erase_;

                for( auto i = orders_ask.begin()
                   ; i != orders_ask.end() && order.limit >= i->first
                   ; ++ i){

                    auto executed_ =
                        std::min(remainder_, i->second.second.quantity);
                    remainder_ -= executed_;

                    if(0 == executed_) {
                        break;
                    }

                    reports.emplace_back(execution_report (
                        execution_report::match,
                        order.side,
                        executed_,
                        direct_order,
                        i->second.second.limit,
                        order.owner
                    ));

                    i->second.second.quantity -= executed_;
                    auto matched_identifier_ = i->second.first;

                    reports.emplace_back(execution_report {
                        execution_report::match,
                        i->second.second.side,
                        executed_, 
                        matched_identifier_,
                        i->second.second.limit,
                        i->second.second.owner
                    });

                    if(0 == i->second.second.quantity){
                        //orders_ask.erase(i);
                        //i = orders_ask.begin();
                        erase_.push_back(i);
                        limit_orders_.erase(matched_identifier_);
                    }
                }

                for(const auto it : erase_) {
                    orders_ask.erase(it);
                }

                if(remainder_ > 0) {
                    // place remainder
                    limit_order placed_ = order;
                    placed_.quantity    = remainder_;
                    auto pair_          = std::make_pair(next_, placed_);
                    orders_bid.emplace(order.limit, pair_);
                    limit_orders_.emplace(next_, order.limit);
                    reports.emplace_back(execution_report {
                        execution_report::placement,
                        order.side, 
                        remainder_,
                        next_,
                        order.limit,
                        order.owner});
                    ++next_;
                }
            }else{
                auto remainder_ = order.quantity;

                std::vector<decltype(orders_bid.begin())> erase_;

                for( auto i = orders_bid.begin()
                    ; i != orders_bid.end() && order.limit <= i->first
                    ; ++ i){

                    auto executed_ = std::min(remainder_, i->second.second.quantity);
                    remainder_ -= executed_;

                    if(0 == executed_) {
                        break;
                    }

                    reports.emplace_back(execution_report {
                        execution_report::match,
                        order.side,
                        executed_,
                        direct_order,
                        i->second.second.limit,
                        order.owner
                    });

                    i->second.second.quantity -= executed_;

                    auto matched_identifier_ = i->second.first;

                    reports.emplace_back(execution_report {
                        execution_report::match,
                        i->second.second.side,
                        executed_,
                        matched_identifier_,
                        i->second.second.limit,
                        i->second.second.owner
                    });

                    if(0 == i->second.second.quantity){
                        //
                        erase_.push_back(i);
                        limit_orders_.erase(matched_identifier_);
                        //i = orders_bid.begin();
                    }
                }

                for(const auto it : erase_) {
                    orders_bid.erase(it);
                }

                if(remainder_ > 0) {
                    limit_order placed_ = order;
                    placed_.quantity    = remainder_;
                    auto pair_          = std::make_pair(next_, placed_);
                    orders_ask.emplace(order.limit, pair_);
                    limit_orders_.emplace(next_, order.limit);

                    reports.emplace_back(execution_report {
                        execution_report::placement,
                        order.side,
                        remainder_,
                        next_,
                        order.limit,
                        order.owner});
                    ++next_;
                }
            }
        }

        /// 
        /// \brief  
        /// 
        void cancel(order_identifier order) override
        {
            auto o = limit_orders_.find(order);
             
            if(limit_orders_.end() != o) {
                
                if(bid().has_value() && bid().value() >= o->second) {
                    // canceled order is a buy
                    auto [begin_, end_] = orders_bid.equal_range(o->second);

                    for(auto i = begin_; i != end_; ++i) {
                        if(i->second.first == order) {
                            reports.emplace_back(execution_report 
                                { execution_report::cancel
                                , limit_order::buy
                                , i->second.second.quantity
                                , order
                                , o->second
                                , i->second.second.owner
                                });
                            orders_bid.erase(i);
                            break;
                        }
                    }

                } else if(ask().has_value() && ask().value() <= o->second) {
                    // canceled order is a sell
                    auto [begin_, end_] = orders_ask.equal_range(o->second);

                    for(auto i = begin_; i != end_; ++i) {
                        if(i->second.first == order) {
                            reports.emplace_back(execution_report 
                                { execution_report::cancel
                                , limit_order::sell
                                , i->second.second.quantity
                                , order
                                , o->second
                                , i->second.second.owner});
                            orders_ask.erase(i);
                            break;
                        }
                    }
                }
            }
        }

        ///
        /// \brief  Lists all active orders
        /// 
        [[nodiscard]] virtual std::vector<typename basic_book::order_identifier> orders() const
        {
            std::vector<typename basic_book::order_identifier>
                result_;
            for(decltype(limit_orders_)::const_iterator i = limit_orders_.begin(); i != limit_orders_.end(); ++i) {
                result_.push_back(i->first);
            }
            return result_;
        }

        void display(std::uint64_t levels) const override
        {

        }



    };
}  // namespace esl::economics::markets::order_book

#endif  // ME_BINARY_TREE_ORDER_BOOK_HPP
