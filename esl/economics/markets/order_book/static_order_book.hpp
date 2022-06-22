/// \file   static_order_book.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-11-03
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
#ifndef ESL_STATIC_ORDER_BOOK_HPP
#define ESL_STATIC_ORDER_BOOK_HPP

#include <cstdint>

#include <esl/economics/markets/order_book/basic_book.hpp>
#include <esl/computation/block_pool.hpp>
#include <esl/data/log.hpp>


namespace esl::economics::markets::order_book {

        
        class static_order_book 
            : public basic_book 
        {
        public:
            typedef std::uint32_t quantity_t_;

            ///
            /// \brief  An abbreviated record for orders in the order book.
            ///
            ///
            struct record
            {
                ///
                /// \brief
                ///
                quote limit;

                ///
                /// \brief  Order quantity remaining
                ///
                quantity_t_ quantity = 0;

                ///
                /// \brief  The market participant that submitted the order.
                ///
                identity<agent> owner;

                ///
                /// \brief  Pointer to the block containing the preceding order in the order-queue
                ///
                computation::block_pool::block<record> *predecessor = nullptr;

                ///
                /// \brief  Pointer to the block containing the succeeding order in the order-queue
                ///
                computation::block_pool::block<record> *successor = nullptr;
            };

            typedef computation::block_pool::static_block_pool<record> pool_t;

            ///
            /// \brief
            ///
            pool_t pool_;

        public:
            typedef typename pool_t::index index;

            typedef std::int64_t limit;

            ///
            /// \brief  Used by the data-structure index the memory pool.
            ///
            typedef computation::block_pool::block<record> *record_pointer;

            ///
            /// \brief  Used by the data-structure to index the most competitive
            ///         order, and the order at the end of the queue at the
            ///         given limit price.
            ///
            typedef std::pair<record_pointer, record_pointer> limit_type;

            ///
            /// \brief  Datastructure used to match price levels to orders.
            ///
            /// \details    `limits_.front()` corresponds to the minimum valid
            ///             price, `limits_.back()` to the maximum valid price.
            ///             One entry per tick.
            ///
            std::vector<limit_type> limits_;

        private:
            ///
            /// \brief  Limit prices below the minimum price or above the maximum
            ///         price are rejected, and when changed, orders outside
            ///         the interval are cancelled directly thereafter.
            ///
            mathematics::interval<quote> valid_limits;

            ///
            /// \brief  pointer into the `limits_` datastructure to the best
            ///         bid offer. Takes `nullptr` value when buy side of the
            ///         order book is empty.
            ///
            limit_type *best_bid_;


            ///
            /// \brief  pointer into the `limits_` datastructure to the best
            ///         ask offer. Takes `nullptr` value when sell side of the
            //          order book is empty.
            ///
            limit_type *best_ask_;

            ///
            /// \brief  Used to translate quotes to a position in the data
            ///         structure.
            /// \returns    Success is communicated by the return value `true`.
            ///
            std::function<bool(const mathematics::interval<quote> &limits, const quote &quote, limit &out_limit)> encode;

            ///
            /// \brief  Used to get the associated quote from a position in the
            ///             data-structure.
            ///
            std::function<quote(const mathematics::interval<quote> &limits, const limit &limit)> decode;

            ///
            /// \brief  Computes the required number of tick levels
            /// \return
            size_t span(size_t lot, size_t ticks) const
            {
                return static_cast<size_t>(
                    (double(valid_limits.upper) - double(valid_limits.lower)) * lot
                    * ticks + 1);
            }

        public:
            ///
            /// \brief      Number of ticks in the currency unit.
            /// \example    When prices are quoted in USD, and to a precision of
            ///             cents, the number of ticks per currency unit is 100.
            ///
            std::uint32_t ticks;

            ///
            /// \brief  The default encoding function
            ///
            /// \param q
            /// \param out_limit
            /// \return
            /*C++20 constexpr*/  bool default_encode(const mathematics::interval<quote> &limits, const quote &q, limit &out_limit)
            {
                if(!limits.contains(q)){
                    return false;
                }

                const auto real_ = double(q);

                const auto high_ = double(limits.upper);
                const auto low_  = double(limits.lower);

                out_limit = static_cast<limit>(
                    round(
                    (real_ - low_)
                    / (high_ - low_)
                    * limits_.size())
                );
                return true;
            }

            ///
            /// \brief  The default decoding function
            ///
            /// \param limit
            /// \return
            /*C++20 constexpr*/ quote default_decode(const mathematics::interval<quote> &limits, const limit &limit)
            {
                auto high_ = double(valid_limits.upper);
                auto low_  = double(valid_limits.lower);

                auto reverse = round((double(limit) * (high_ - low_)) * limits.lower.lot * limits.lower.lot);

                auto intercept_ = low_ * limits.lower.lot * limits.lower.lot;
                return quote( (reverse/ double(limits_.size()-1) +  intercept_) / limits.lower.lot, valid_limits.lower);
            }

            ///
            /// \brief
            ///
            /// \details    The tick-size is set to the minimum lot size taken
            ///             from the minimum and maximum quotes
            ///
            /// \param minimum
            /// \param maximum
            /// \param capacity
            ///
            static_order_book(const quote &minimum
                             , const quote &maximum
                             , std::uint32_t capacity = 128*1024
                             )
            : basic_book( )
            , pool_(capacity)
            , valid_limits(minimum, maximum)
            , ticks(std::min(static_cast<std::uint32_t >(minimum.lot),
                             static_cast<std::uint32_t>(maximum.lot)))
            {

                reports.reserve(32);
                if(valid_limits.empty()){
                    throw esl::exception("invalid limits specified");
                }
                if(minimum.lot != maximum.lot){
                    throw esl::exception("different lot sizes between minimum and maximum");
                }

                // +1 because the maximum value is included
                auto span_ = span(minimum.lot, ticks);
                // since nullptr is used in the logic of the datastructure,
                //  we make sure to set this explicitly
                limits_.resize(span_, std::make_pair(nullptr, nullptr));

                best_bid_ = &limits_.front();
                best_ask_ = &limits_.back();

                encode = [&](const mathematics::interval<quote> &limits, const auto &q, auto &l) -> bool {
                    return default_encode(limits, q, l);
                };

                decode = [&](const mathematics::interval<quote> &limits, const auto &l) -> quote {
                    return default_decode(limits, l);
                };
            }

            ///
            /// \brief  Resize the order book when market prices move outside of
            ///         the allowed range.
            ///
            /// \details    This operation is costly, and must be avoided by
            ///             choosing sensible limits for minimum and maximum
            ///             allowed prices.
            ///
            ///
            /// \param new_limits
            void resize(const mathematics::interval<quote> &new_limits)
            {
                // this is a naive implementation, that may hit memory limits
                // quickly (worst case, resizing from n to size m, it uses n+m
                // while resizing).

                throw esl::exception("not implemented");

                std::vector<limit_type> new_limits_;

                // TODO: implement

                valid_limits = new_limits;
            }

            /// 
            /// \brief lists all open orders
            /// 
            [[nodiscard]] std::vector<basic_book::order_identifier> orders() const
            {
                std::vector<basic_book::order_identifier> result_;


                for(auto i = &limits_[0]; i <= &limits_[limits_.size() - 1]; ++i) {
                    for(auto o = i->first; o; o = o->data.successor) {
                        result_.push_back(o->index);
                        if(o->data.successor == o) {
                            break;
                        }
                    }
                }

                return result_;
            }



            ///
            /// \brief  Returns the best bid, the highest bid price, if any
            ///
            /// \return
            [[nodiscard]] std::optional<quote> bid() const override
            {
                if(!best_bid_->first){
                    return {};
                }
                std::ptrdiff_t limit_ = best_bid_ - &limits_[0];
                return decode(valid_limits, limit_);
            }

            ///
            /// \brief  Returns the best ask, meaning lowest ask price, if any
            ///
            /// \return
            [[nodiscard]] std::optional<quote> ask() const override
            {
                if(!best_ask_->first){
                    return {};
                }

                std::ptrdiff_t limit_ = best_ask_ - &limits_[0];
                return decode(valid_limits, limit_);
            }

            ///
            /// \brief  Matches an order, partially or fully, agains resting
            ///         orders at a given level in the book
            ///
            /// \param order
            /// \param remainder_
            /// \param level
            /// \return
            inline uint32_t match_at_level( const limit_order &order
                                          , std::uint32_t &remainder_
                                          , limit_type *level)
            {
                for(auto ao = level->first; 0 < remainder_; ao = ao->data.successor){
                    uint32_t execution_size_ = 0;

                    const bool exhausted_ = ao->data.quantity <= remainder_;

                    if(!exhausted_) {
                        // we can meet the entire order, so we finish here
                        execution_size_ = remainder_;
                        ao->data.quantity -= execution_size_;
                        remainder_ = 0;
                    }else{
                        execution_size_ = ao->data.quantity;
                        remainder_ -= execution_size_;
                        ao->data.quantity = 0;

                        // this exhausted the highest-priority order, so we should now erase it
                        if(ao->data.successor) {
                            // the preceding order was exhausted
                            ao->data.successor->data.predecessor = nullptr;
                        }
                        // whether there is one or not, move the pointer in the index 
                        level->first = ao->data.successor;

                    }

                    auto quote_ = decode(valid_limits, level - &limits_[0]);
                    // execution report for liquidity taker
                    reports.emplace_back(execution_report
                                             ( execution_report::match
                                             , order.side
                                             , execution_size_
                                             , basic_book::direct_order
                                             , quote_
                                             , order.owner
                                             ));

                    // execution report for supplier
                    reports.emplace_back(execution_report
                                             ( execution_report::match
                                             , (order.side == limit_order::sell ? limit_order::buy : limit_order::sell)
                                             ,  execution_size_
                                             , ao->index
                                             , quote_
                                             , ao->data.owner
                                             ));

                    auto successor_ = ao->data.successor;
                    
                    if(exhausted_) {
                        //std::cout << "\t\t exhausted order " << ao->index << std::endl;
                        pool_.erase(ao->index);
                    }

                    // are there lower priority orders on this level? if so, continue
                    if(successor_){
                        continue;
                    }
                    
                    // if all resting orders were exhausted, we have reached the end of the level. 
                    // Thus if we emptied the last order, then we need to clear our the level index
                    // and update the best bid and ask
                    if(exhausted_){
                        // special case where level is emptied entirely
                        // we need to reset the pointers in the memory pool
                        level->first = nullptr;
                        level->second = nullptr;

                        // if the aggressor was a buy order, it took away
                        // the best ask
                        if(order.side == limit_order::buy){
                            // increase the best ask, until we find an order, or exhaust the order book
                            // TODO: worst_ask_
                            for(++best_ask_; best_ask_ <= &limits_.back(); ++best_ask_){
                                if(best_ask_->first || best_ask_ == &limits_.back()){
                                    break;
                                }
                            }
                        }else{
                            // decrease the best bid, until we find an order, or exhaust the order book
                            // TODO: worst_bid_
                            for(--best_bid_; best_bid_ >= &limits_[0]; --best_bid_){
                                if(best_bid_->first || best_bid_ == &limits_[0]){
                                    break;
                                }
                            }
                        }
                    }
                    break;
                    
                }
                return remainder_;
            }

            ///
            /// \brief
            ///
            /// \param order
            /// \return
            void insert(const limit_order &order) override
            {
                if(!valid_limits.contains(order.limit) || 0 >= order.quantity || order.limit.lot != valid_limits.lower.lot ){

                    if(!valid_limits.contains(order.limit)){
                        LOG(warning) << "Order invalid because it is outside the accepted range: " << this->valid_limits << std::endl;
                    }

                    if(0 >= order.quantity){
                        LOG(warning) << "Order invalid because it does not "
                                        "have positive quantity "
                                     << order.quantity << std::endl;
                    }

                    if(order.limit.lot != valid_limits.lower.lot){
                        LOG(warning)
                            << "Order invalid because it does not use an "
                               "acceptable lot size "
                            << order.limit.lot
                            << " !=  " << valid_limits.lower.lot << std::endl;
                    }

                    reports.emplace_back(execution_report
                                             ( execution_report::invalid
                                             , order.side
                                             , order.quantity
                                             , basic_book::direct_order
                                             , order.limit
                                             , order.owner
                                             ));
                    return;
                }

                std::uint32_t remainder_ = order.quantity;
                limit limit_index_;
                auto encode_success_ = encode(valid_limits, order.limit, limit_index_);

                if(!encode_success_){
                    throw esl::exception("quote not mapped to valid order book index");
                }
                if(limit_index_ < 0) {
                    throw esl::exception("quote maps to index below zero");
                }
                if(limits_.size() <= static_cast<uint64_t>(limit_index_)){
                    throw esl::exception("quote maps to index beyond maximum value");
                }

                limit_type *limit_level_ = &limits_[limit_index_];

                if(order.side == limit_order::buy && ask().has_value() && order.limit >= ask().value()) {
                    // direct execution: buyer aggressor
                    for(auto al = best_ask_; al <= limit_level_ && 0 < remainder_; ++al){
                        if(!al->first){
                            continue;
                        }
                        remainder_ = match_at_level(order, remainder_, al);
                    }
                }else if(order.side == limit_order::sell && bid().has_value() && order.limit <= bid().value()) {
                    // direct execution: seller aggressor
                    for(auto bl = best_bid_; bl >= limit_level_ && 0 < remainder_; --bl){
                        if(!bl->first){
                            continue;
                        }
                        remainder_ = match_at_level(order, remainder_, bl);
                    }

                }else if(order.lifetime == limit_order::lifetime_t::immediate_or_cancel
                           || order.lifetime == limit_order::lifetime_t::fill_or_kill){
                    // cancel an immediate/fill order that could not be matched
                    reports.emplace_back(execution_report
                                             ( execution_report::cancel
                                             , order.side
                                             , order.quantity
                                             , basic_book::direct_order
                                             , order.limit
                                             , order.owner
                                                 ));
                    return;
                }

                // placement of remainder_. Could be all of the original order,
                // or if not "immediate or cancel", a partial remaining order
                if(0 >= remainder_){
                    return;
                }
                if(order.lifetime
                   == limit_order::lifetime_t::immediate_or_cancel) {
                    reports.emplace_back(execution_report
                                             ( execution_report::cancel
                                             , order.side
                                             , remainder_
                                             , basic_book::direct_order
                                             , order.limit
                                             , order.owner
                                             ));
                    return;
                }

                auto block_ = pool_.emplace(record
                                                { order.limit
                                                , remainder_
                                                , order.owner
                                                , nullptr
                                                , nullptr
                                                });
                
                // we store the identifier in the record
                if(block_.second) {
                    block_.second->index = block_.first;
                }
                
                reports.emplace_back(execution_report
                                         ( execution_report::placement
                                         , order.side
                                         , remainder_
                                         , block_.first
                                         , order.limit
                                         , order.owner
                                         ));

                // if this is the first order at this level, then the first of the pair of pointers is null
                if(!limit_level_->first){
                    // point the main index to the order
                    limit_level_->first = block_.second;
                    // point the rear of the main index to this order
                    limit_level_->second = block_.second;
                }else{
                    // we assert that because there is at least one order, the second pointer of the pair
                    // is set too to the least priority order at this level
                    assert(limit_level_->second != nullptr);
                    // the previously least priority order now is the second to last. Thus, that order's successor is the new order:
                    assert(limit_level_->second->data.successor !=
                               block_.second);
                    limit_level_->second->data.successor = block_.second;
                    // and the new order's predecessor is the previous least priority order:
                    block_.second->data.predecessor = limit_level_->second;
                    // point the rear of the level pointers to the new order
                    limit_level_->second = block_.second;
                }

                // because we are inserting, the best bid and ask can strictly improve
                if(limit_order::buy == order.side){
                    best_bid_ = std::max(best_bid_,  limit_level_);
                }else if(order.side == limit_order::sell){
                    best_ask_ = std::min(best_ask_,  limit_level_);
                }
            }


            ///
            /// \brief  Cancels an order by the order identifier returned from
            ///         the order book.
            ///
            /// \param order
            void cancel(basic_book::order_identifier order) override
            {
                record &order_ = pool_[order];

                auto side_ = limit_order::side_t::buy;
                // if there is no bid, or the best bid is lower than this order,
                // this must be a sell order.
                auto bid_ = bid();
                if(!bid_.has_value() || bid_.value() < order_.limit){
                    side_ = limit_order::side_t::sell;
                }

                limit limit_index_;
                auto encode_success_ = encode(valid_limits, order_.limit, limit_index_);
                limit_type *limit_level_ = &limits_[limit_index_];

                
                // The order could be cancelled/matched before, and nothing is left at this level
                if(!limit_level_->first || !limit_level_->second) {
                    return;
                }


                if(limit_level_->first->index == order) {
                    limits_[limit_index_].first =
                        limit_level_->first->data.successor;
                }


                if(limit_level_->second->index == order) {
                    limits_[limit_index_].second =
                        limit_level_->second->data.predecessor;
                }

                // either both were set to an order
                bool both_set_ = (limits_[limit_index_].first
                                  && limits_[limit_index_].second);
                // or both are empty
                bool none_set_ = (!limits_[limit_index_].first
                                  && !limits_[limit_index_].second);

                assert(both_set_ || none_set_ );

                auto result_ = pool_.erase(order);

                // canceled
                if(0 == result_) {
                    // the order was matched fully/cancelled before, so we can't
                    // do anything here
                    //LOG(warning) << "canceled dead order << " << order << std::endl;
                    return;
                }
/*
                std::cout << "cancelling an order of " << order_.quantity
                          << " times " << order_.limit << std::endl; */ 
                reports.emplace_back(execution_report
                                         ( execution_report::cancel
                                         , side_
                                         , order_.quantity
                                         , order
                                         , order_.limit
                                         , order_.owner
                                         ));

                if(order_.predecessor) {
                    order_.predecessor->data.successor = order_.successor; 
                }

                if(order_.successor) {
                    order_.successor->data.predecessor = order_.predecessor;
                }

                if(order_.predecessor) {
                    order_.predecessor = nullptr;
                }

                if(order_.successor) {
                    order_.successor = nullptr;
                }

                order_.limit    = quote();
                order_.owner    = identity<agent>();
                order_.quantity = 0;
            }

            ///
            /// \brief  Displays a debugging view for the order book,
            ///         for use in a terminal/IDE.
            ///
            /// \param levels
            void display(std::uint64_t levels = 5) const
            {
                std::cout << "            bid |                | ask            " << std::endl;

                std::cout << std::setfill(' ');

                std::uint64_t displayed_ = 0;
                std::vector<std::pair<std::uint32_t, double>> ask_displayed_;
                for(auto i = best_ask_; i <= &limits_[limits_.size() - 1] && displayed_ < levels; ++i) {
                    if(!i->first) {
                        continue;
                    }
                    std::uint64_t quantity_ = 0;
                    for(auto *j = i->first; nullptr != j; j = j->data.successor) {
                        quantity_ += j->data.quantity;
                    }
                    if(quantity_ <= 0) {
                        continue;
                    }

                    ask_displayed_.emplace_back(quantity_,double(decode(valid_limits, i - &limits_[0]))* valid_limits.lower.lot);

                    ++displayed_;
                }

                for(auto r = ask_displayed_.rbegin(); r!= ask_displayed_.rend(); ++r){
                    std::cout << "                | "
                              << std::left << std::setw(14) << std::setprecision(int(std::log10(valid_limits.lower.lot)))
                        << std::fixed << r->second
                              << " | "
                              << std::left << std::setw(15)
                              << r->first
                              << std::endl;
                }

                //std::cout << "------------------------------------------------" << std::endl;

                displayed_ = 0;
                for(auto i = best_bid_; i >= &limits_[0] && displayed_ < levels; --i) {
                    if(!i->first) {
                        continue;
                    }
                    std::uint64_t quantity_ = 0;
                    for(auto *j = i->first; nullptr != j; j = j->data.successor) {
                        quantity_ += j->data.quantity;
                    }
                    if(quantity_ <= 0) {
                        continue;
                    }
                    std::cout << std::right << std::setw(15)
                              << quantity_ << " | "
                              << std::left << std::setw(14)
                        << std::setprecision(int(std::log10(valid_limits.lower.lot)))
                        << double(decode(valid_limits, i - &limits_[0])) * valid_limits.lower.lot
                        //<< double(decode(valid_limits, i - &limits_[0])) * valid_limits.lower.lot
                              << " | "
                              << std::endl;
                    ++displayed_;
                }

            }

        };
}//namespace

#endif  // ESL_STATIC_ORDER_BOOK_HPP
