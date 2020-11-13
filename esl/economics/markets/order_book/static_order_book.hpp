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
                quantity_t_ quantity;

                ///
                /// \brief  The market participant that submitted the order.
                ///
                identity<agent> owner;

                ///
                /// \brief  Pointer to the next block in the order-queue
                ///
                computation::block_pool::block<record> *successor;
            };

            typedef computation::block_pool::static_block_pool<record> pool_t;

            pool_t pool_;

        public:
            typedef typename pool_t::index index;

            typedef std::int64_t limit;

            typedef computation::block_pool::block<record> *record_pointer;
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
            ///         bid offer
            ///
            limit_type *best_bid_;


            limit_type *best_ask_;

            ///
            /// \brief  Used to translate quotes to a position in the data
            ///         structure.
            /// \returns    Success is communicated by the return value `true`.
            ///
            std::function<bool(const quote &quote, limit &out_limit)> encode;

            ///
            /// \brief
            ///
            std::function<quote(const limit &limit)> decode;

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
            constexpr bool default_encode(const quote &q, limit &out_limit)
            {
                if(valid_limits.lower > q || valid_limits.upper < q){
                    return false;
                }
                out_limit = static_cast<limit>(
                    (double(q) - double(valid_limits.lower))
                    / (double(valid_limits.upper) - double(valid_limits.lower))
                    * limits_.size()
                );
                return true;
            }

            ///
            /// \brief  The default decoding function
            ///
            /// \param limit
            /// \return
            /*C++20 constexpr*/ quote default_decode(const limit &limit)
            {
                auto reverse = (double(limit) * (double(valid_limits.upper) - double(valid_limits.lower))) * valid_limits.lower.lot * valid_limits.lower.lot;
                auto intercept_ = double(valid_limits.lower) * valid_limits.lower.lot * valid_limits.lower.lot;
                return quote( ((reverse )/ (limits_.size()-1) +  intercept_) / valid_limits.lower.lot, valid_limits.lower);
            }

            ///
            /// \brief
            ///
            /// \details    The tick-size is set to the minimum lot size taken
            ///             from the minimum and maximum quotes, i.e.
            ///             `ticks = min(minimum.lot, maximum.lot)`
            ///
            /// \param minimum
            /// \param maximum
            /// \param capacity
            ///
            static_order_book( const quote &minimum
                , const quote &maximum
                , size_t capacity = 128*1024
            )
                : basic_book( )
                , pool_(capacity)
                , valid_limits(minimum, maximum)
                , ticks(std::min(minimum.lot, maximum.lot))
            {
                reports.reserve(32);
                assert(!valid_limits.empty());
                assert(minimum.lot == maximum.lot);
                // +1 because the maximum value is included
                auto span_ = static_cast<size_t>(
                    (double(valid_limits.upper) - double(valid_limits.lower)) * minimum.lot
                    * ticks + 1);
                // since nullptr is used in the logic of the datastructure,
                //  we make sure to set this explicitly
                limits_.resize(span_, std::make_pair(nullptr, nullptr));

                best_bid_ = &limits_.front();
                best_ask_ = &limits_.back();

                encode = [&](const auto &q, auto &l) -> bool {
                    return default_encode(q, l);
                };

                decode = [&](const auto &l) -> quote {
                    return default_decode(l);
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
            void resize(mathematics::interval<quote> new_limits)
            {
                throw std::logic_error("Not implemented");

                /*
                if(new_limits.lower > valid_limits.lower && new_limits.lower <= valid_limits.upper){
                    // safely encode and erase orders in between
                    limit lower_;
                    encode(new_limits.lower, lower_);

                    for(auto i = &limits_[0]; i < lower_; ++i){
                        for(auto j = i->first; nullptr != j; j = j->data.successor){
                            // TODO: investigate fast memcpy
                        }
                    }

                }

                // other cases
                if(new_limits.upper < valid_limits.upper && new_limits.upper > valid_limits.lower) {
                    // TODO:
                }

                valid_limits = new_limits;*/
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
                return decode(limit_);
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
                return decode(limit_);
            }

            ///
            /// \brief  Matches an order, partially or fully, agains resting
            ///         orders at a given level in the book
            ///
            /// \param order
            /// \param remainder_
            /// \param level
            /// \return
            inline uint32_t match_at_level( const limit_order_message &order
                , std::uint32_t &remainder_
                , limit_type *level)
            {
                for( auto ao = level->first
                    ; 0 < remainder_
                    ; ao = ao->data.successor){
                    uint32_t execution_size_ = 0;

                    if(ao->data.quantity > remainder_){
                        execution_size_ = remainder_;
                        ao->data.quantity -= execution_size_;
                        remainder_ = 0;
                    }else{
                        execution_size_ = ao->data.quantity;
                        remainder_ -= execution_size_;
                        ao->data.quantity = 0;
                        level->first = ao->data.successor;
                    }

                    auto quote_ = decode(level - &limits_[0]);

                    // execution report for liquidity taker
                    reports.emplace_back(execution_report
                                             { execution_report::match
                                             , execution_size_
                                             , basic_book::direct_order
                                             , order.side
                                             , quote_
                                             , order.owner
                                             });

                    // execution report for supplier
                    reports.emplace_back(execution_report
                                             { execution_report::match
                                             , execution_size_
                                             , ao->index
                                             , (order.side == limit_order_message::sell ?   limit_order_message::buy :  limit_order_message::sell), quote_
                                             , ao->data.owner
                                             });

                    if(!ao->data.successor){
                        if(0 == ao->data.quantity){
                            // special case where level is emptied entirely
                            // we need to reset the pointers in the memory pool
                            level->first = nullptr;
                            level->second = nullptr;

                            // if the aggressor was a buy order, it took away
                            // the best ask
                            if(order.side == limit_order_message::buy){
                                LOG(trace) << quote_ << " ask level depleted" << std::endl;
                                for(++best_ask_; best_ask_ <= &limits_.back(); ++best_ask_){
                                    if(best_ask_->first || best_ask_ == &limits_.back()){
                                        break;
                                    }
                                }
                            }else{
                                LOG(trace) << quote_ << " bid level depleted" << std::endl;
                                for(--best_bid_; best_bid_ >= &limits_[0]; --best_bid_){
                                    if(best_bid_->first || best_bid_ == &limits_[0]){
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
                return remainder_;
            }
            ///
            /// \brief
            ///
            /// \param order
            /// \return
            void insert(const limit_order_message &order)
            {
                if(!valid_limits.contains(order.limit) || 0 >= order.quantity ){
                    reports.emplace_back(execution_report
                                             { execution_report::invalid
                                             , order.quantity
                                             , basic_book::direct_order
                                             , order.side
                                             , order.limit
                                             , order.owner
                                             });
                    return;
                }

                std::uint32_t remainder_ = order.quantity;
                limit limit_index_;
                auto encode_success_ = encode(order.limit, limit_index_);
                assert( encode_success_
                        && limit_index_ >= 0
                        && limits_.size() > static_cast<uint64_t>(limit_index_));
                limit_type *limit_level_ = &limits_[limit_index_];

                if( order.side == limit_order_message::buy
                    && ask().has_value()
                    && order.limit >= ask().value()) {
                    // direct execution: buyer aggressor
                    LOG(trace) << "buyer aggressor" << std::endl;
                    for(auto al = best_ask_; al <= limit_level_ && 0 < remainder_; ++al){
                        if(!al->first){
                            continue;
                        }
                        remainder_ = match_at_level(order, remainder_, al);
                    }

                }else if(  order.side == limit_order_message::sell
                           && bid().has_value()
                    ){
                    // direct execution: seller aggressor
                    LOG(trace) << "seller aggressor" << std::endl;
                    for(auto bl = best_bid_; bl >= limit_level_ && 0 < remainder_; --bl){
                        if(!bl->first){
                            continue;
                        }
                        LOG(trace) << "\t ask " << remainder_ << " units found bid(s) at " << (decode(bl - &limits_[0])) << std::endl;
                        remainder_ = match_at_level(order, remainder_, bl);
                    }

                }else if(  order.lifetime == limit_order_message::immediate_or_cancel
                           || order.lifetime == limit_order_message::fill_or_kill){
                    // cancel an immediate/fill order that could not be matched
                    reports.emplace_back(execution_report
                                             { execution_report::cancel
                                             , order.quantity
                                             , basic_book::direct_order
                                             , order.side
                                             , order.limit
                                             , order.owner
                                             });
                    return;
                }

                // placement of remainder_. Could be all of the original order,
                // or if not "immediate or cancel", a partial remaining order
                if(0 >= remainder_){
                    return;
                }
                if(order.lifetime == limit_order_message::immediate_or_cancel){
                    reports.emplace_back(execution_report
                                             { execution_report::cancel
                                             , remainder_
                                             , basic_book::direct_order
                                             , order.side
                                             , order.limit
                                             , order.owner
                                             });
                    return;
                }

                auto block_ = pool_.emplace(record
                                                { order.limit
                                                , remainder_
                                                , order.owner
                                                , nullptr
                                                });

                reports.emplace_back(execution_report
                                         { execution_report::placement
                                         , remainder_
                                         , block_.first
                                         , order.side
                                         , order.limit
                                         , order.owner
                                         });

                if(!limit_level_->first){
                    limit_level_->first = block_.second;
                    limit_level_->second = block_.second;
                }else{
                    limit_level_->second->data.successor = block_.second;
                    limit_level_->second = block_.second;
                }

                if(limit_order_message::buy == order.side){
                    best_bid_ = std::max(best_bid_,  limit_level_);
                }else if(order.side == limit_order_message::sell){
                    best_ask_ = std::min(best_ask_,  limit_level_);
                }
                // TODO: notify new best bid/ask
            }


            ///
            /// \brief  Cancels an order by the order identifier returned from
            ///         the order book.
            ///
            /// \param order
            void cancel(basic_book::order_identifier order)
            {
                auto index_ = (std::uint64_t)( order );

                const record &order_ = pool_[order];

                auto side_ = limit_order_message::side_t::buy;
                // if there is no bid, or the best bid is lower than this order,
                // this must be a sell order
                auto best_bid_ = bid();
                if(!best_bid_.has_value() || best_bid_.value() < order_.limit){
                    side_ = limit_order_message::side_t::sell;
                }

                reports.emplace_back(execution_report
                                         { execution_report::cancel
                                         , order_.quantity
                                         , order
                                         , side_
                                         , order_.limit
                                         , order_.owner
                                         });

                pool_.erase(index_);
            }

            ///
            /// \brief  Displays a debugging view for the order book,
            ///         for use in a terminal/IDE.
            ///
            /// \param levels
            void display(std::uint64_t levels = 5) const
            {
                std::cout << "            bid |                | ask            "
                          << std::endl;

                std::uint64_t displayed_ = 0;
                std::vector<std::pair<std::uint32_t, double>> ask_displayed_;
                for(auto i = best_ask_; i <= &limits_[limits_.size() - 1] && displayed_ < levels; ++i) {
                    if(!i->first) {
                        continue;
                    }
                    std::uint64_t quantity_ = 0;
                    for(auto *j = i->first; nullptr != j;
                        j       = j->data.successor) {
                        quantity_ += j->data.quantity;
                    }
                    if(quantity_ <= 0) {
                        continue;
                    }

                    ask_displayed_.emplace_back(quantity_,double(decode(i - &limits_[0]))*100*100);
                    /*std::cout << "                | "
                              << std::left << std::setw(14)
                              << std::setprecision(int(std::log10(valid_limits.lower.lot ))) << double(decode(i - &limits_[0]))*100*100
                              << " | "
                              << std::left << std::setw(15)
                              << quantity_
                              << std::endl;*/
                    ++displayed_;
                }

                for(auto r = ask_displayed_.rbegin(); r!= ask_displayed_.rend(); ++r){
                    std::cout << "                | "
                              << std::left << std::setw(14)
                              << std::setprecision(int(std::log10(valid_limits.lower.lot ))) << r->second
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
                    for(auto *j = i->first; nullptr != j;
                        j       = j->data.successor) {
                        quantity_ += j->data.quantity;
                    }
                    if(quantity_ <= 0) {
                        continue;
                    }
                    std::cout << std::right << std::setw(15)
                              << quantity_ << " | "
                              << std::left << std::setw(14)
                              << std::setprecision(int(std::log10(valid_limits.lower.lot  ))) << double(decode(i - &limits_[0]))*100*100
                              << " | "
                              << std::endl;
                    ++displayed_;
                }

            }

        };
}//namespace

#endif  // ESL_STATIC_ORDER_BOOK_HPP
