/// \file   book.hpp
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


#ifndef ME_ORDER_BOOK_HPP
#define ME_ORDER_BOOK_HPP

#include <map>
#include <queue>
#include <string>


#include <esl/economics/markets/order_book/order.hpp>
#include <esl/economics/markets/quote.hpp>
#include <esl/computation/block_pool.hpp>
#include <esl/mathematics/interval.hpp>
#include <utility>


namespace esl::economics::markets::order_book {




    struct basic_book
    {

        basic_book( )

        {

        }

        virtual ~basic_book() = default;
    };


    namespace dynamically_allocated {

        /*
        class book
        {
        public:
            typedef std::uint64_t index;

        private:
            index next_;

        public:
            book()
            :next_(0)
            {

            }

            bool insert(const limit_order_message &order);

            void erase(index identifier);

            limit_order_message &find( limit_order_message::side_t side
                                     , index identifier);

            bool match(std::queue<limit_order_message> &);

            [[nodiscard]] price spread() const
            {
                return orders_ask.cbegin()->first - orders_bid.cbegin()->first;
            }

            void match(limit_order_message &bid, limit_order_message &ask);

            typedef std::multimap< price
                                 , std::pair<index, limit_order_message>
                                 , std::greater<>
                                 > bid_t;

            typedef std::multimap< price
                                 , std::pair<index, limit_order_message>
                                 , std::less<>
                                 > ask_t;

            bid_t orders_bid;
            ask_t orders_ask;
        };
         */
    }//dynamically_allocated

    ///
    ///
    ///
    namespace statically_allocated {

        struct execution_report
        {
            enum
            {   invalid
                ,   cancel
                ,   match
                ,   placement
            } state : 2;
            std::uint64_t               quantity;
            std::uint64_t               identifier;
            limit_order_message::side_t side;           // possibly superfluous
            quote                       limit;
            identity<agent>             owner;
        };

        template<size_t max_orders_ = 0x1ull<<17
                , typename quantity_t_ = std::uint32_t >
        class book
        : public basic_book
        {
        public:
            struct record
            {
                quantity_t_ quantity;
                identity<agent> owner;
                computation::block_pool::block<record> *successor;
            };

            typedef computation::block_pool::static_block_pool<record, max_orders_> pool_t;

            pool_t pool_;

        public:
            typedef typename pool_t::index index;

            typedef std::int64_t limit;

            /*//
            /// \brief      Number of ticks in the currency unit.
            /// \example    When prices are quoted in USD, and to a precision of
            ///             cents, the number of ticks per currency unit is 100.
            ///
            std::uint32_t ticks;
             */


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
            std::uint32_t ticks;

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

            /*C++20 constexpr*/constexpr quote default_decode(const limit &limit)
            {
                auto reverse = (double(limit) * (double(valid_limits.upper) - double(valid_limits.lower))) * valid_limits.lower.lot * valid_limits.lower.lot;
                auto intercept_ = double(valid_limits.lower) * valid_limits.lower.lot * valid_limits.lower.lot;
                return quote( ((reverse )/ (limits_.size()-1) +  intercept_) / valid_limits.lower.lot, valid_limits.lower);
            }

            std::queue<execution_report> executions;

            ///
            ///
            ///
            /// \param minimum
            /// \param maximum
            /// \param decimals
            book( quote minimum
                , quote maximum
                //, std::uint32_t ticks = 10'000
                //, const std::function<bool(const quote &quote, limit &out_limit)>& encode = default_encode
                //, std::function<quote(const limit &limit)> decode = default_decode
                )
            : basic_book( )
            , valid_limits(minimum, maximum)
            , pool_()
            //, encode(encode)
            //, decode(decode)
            , ticks(minimum.lot)
            {
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

            [[nodiscard]] std::optional<quote> bid() const
            {
                if(!best_bid_->first){
                    return {};
                }
                std::ptrdiff_t limit_ = best_bid_ - &limits_[0];
                return decode(limit_);
            }

            [[nodiscard]] std::optional<quote> ask() const
            {
                if(!best_ask_->first){
                    return {};
                }

                std::ptrdiff_t limit_ = best_ask_ - &limits_[0];
                return decode(limit_);
            }

            ///
            /// \brief
            ///
            /// \param order
            /// \return
            void insert(const limit_order_message &order)
            {
                if(!valid_limits.contains(order.limit) || 0 >= order.quantity ){
                    executions.push(execution_report
                        { .state      = execution_report::invalid
                        , .quantity   = order.quantity
                        , .identifier = std::numeric_limits<std::uint64_t>::max()
                        , .side       = order.side
                        , .limit      = order.limit
                        , .owner      = order.owner
                    });
                    return;
                }

                auto remainder_ = order.quantity;
                limit limit_index_;
                auto encode_success_ = encode(order.limit, limit_index_);
                assert(encode_success_ && limits_.size() > limit_index_);
                limit_type *limit_level_ = &limits_[limit_index_];

                if( order.side == limit_order_message::buy
                    && ask().has_value()
                    && order.limit >= ask().value()){
                    // direct execution: buyer aggressor
                    std::cout << "buyer aggressor" << std::endl;
                    for(auto al = best_ask_; al <= limit_level_ && 0 < remainder_; ++al){
                        if(!al->first){
                            continue;
                        }

                        for( auto ao = al->first
                           ; nullptr != ao->data.successor && 0 < remainder_
                           ; ao = ao->data.successor){
                            std::uint64_t execution_size_ = 0;

                            if(ao->data.quantity > remainder_){
                                execution_size_ = remainder_;
                                ao->data.quantity -= execution_size_;
                                remainder_ = 0;
                            }else{
                                execution_size_ = ao->data.quantity;
                                remainder_ -= execution_size_;
                                ao->data.quantity = 0;
                                al->first = ao->data.successor;
                            }

                            auto quote_ = decode(al - &limits_[0]);

                            // execution report for aggressor
                            executions.push(execution_report
                            { .state      = execution_report::match
                            , .quantity   = execution_size_
                            , .identifier = std::numeric_limits<std::uint64_t>::max()
                            , .side       = order.side
                            , .limit      = quote_
                            , .owner      = order.owner
                            });

                            // execution report for supplier
                            executions.push(execution_report
                            { .state      = execution_report::match
                            , .quantity   = execution_size_
                            , .identifier = ao->index
                            , .side       = limit_order_message::sell
                            , .limit      = quote_
                            , .owner      = ao->data.owner
                            });

                            if(0 == ao->data.quantity && !ao->data.successor){
                                // special case where level is emptied entirely
                                al->first = nullptr;
                                al->second = nullptr;
                            }
                        }
                    }
                }else if(  order.side == limit_order_message::sell
                        && bid().has_value()
                        //&& order.limit <= bid().value()
                          ){
                    std::cout << order.limit << " <-> " <<bid().value() << std::endl;


                    // direct execution: seller aggressor
                    std::cout << "seller aggressor" << std::endl;
                    for(auto bl = best_bid_; bl >= limit_level_ && 0 < remainder_; --bl){
                        if(!bl->first){
                            continue;
                        }

                        for( auto bo = bl->first
                            ; nullptr != bo->data.successor && 0 < remainder_
                            ; bo = bo->data.successor){
                            std::uint64_t execution_size_ = 0;

                            if(bo->data.quantity > remainder_){
                                execution_size_ = remainder_;
                                bo->data.quantity -= execution_size_;
                                remainder_ = 0;
                            }else{
                                // order at front of queue is depleted. move front of queue pointer
                                execution_size_ = bo->data.quantity;
                                remainder_ -= execution_size_;
                                bo->data.quantity = 0;
                                bl->first = bo->data.successor;
                            }

                            auto quote_ = decode(bl - &limits_[0]);

                            // execution report for aggressor
                            executions.push(execution_report
                            { .state      = execution_report::match
                            , .quantity   = execution_size_
                            , .identifier = std::numeric_limits<std::uint64_t>::max()
                            , .side       = order.side
                            , .limit      = quote_
                            , .owner      = order.owner
                            });

                            // execution report for supplier
                            executions.push(execution_report
                            { .state      = execution_report::match
                            , .quantity   = execution_size_
                            , .identifier = bo->index
                            , .side       = limit_order_message::buy
                            , .limit      = quote_
                            , .owner      = bo->data.owner
                            });


                            if(0 == bo->data.quantity && !bo->data.successor){
                                // special case where level is emptied entirely
                                bl->first = nullptr;
                                bl->second = nullptr;
                            }
                        }
                    }

                }else if(  order.lifetime == limit_order_message::immediate_or_cancel
                        || order.lifetime == limit_order_message::fill_or_kill){
                        // cancel an immediate/fill order that could not be matched
                        executions.push(execution_report
                            { .state    = execution_report::cancel
                            , .quantity = order.quantity
                            , .identifier = std::numeric_limits<std::uint64_t>::max()
                            , .side     = order.side
                            , .limit    = order.limit
                            , .owner    = order.owner
                            });
                        return;
                }

                // placement of remainder_. Could be all of the original order,
                // or if not "immediate or cancel", a partial remaining order
                if(0 >= remainder_){
                    return;
                }
                if(order.lifetime == limit_order_message::immediate_or_cancel){
                    executions.push(execution_report
                    { .state        = execution_report::cancel
                    , .quantity     = remainder_
                    , .identifier   = std::numeric_limits<std::uint64_t>::max()
                    , .side         = order.side
                    , .limit        = order.limit
                    , .owner        = order.owner
                    });
                    return;
                }

                auto block_ = pool_.emplace(record
                      { .quantity  = order.quantity
                      , .owner     = order.owner
                      , .successor = nullptr
                });

                executions.push(execution_report
                { .state        = execution_report::placement
                , .quantity     = remainder_
                , .identifier   = block_.first
                , .side         = order.side
                , .limit        = order.limit
                , .owner        = order.owner
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
                    std::cout << bid().value() << std::endl;
                }else if(order.side == limit_order_message::sell){
                    best_ask_ = std::min(best_ask_,  limit_level_);
                    std::cout << ask().value() << std::endl;
                }
                // TODO: notify new best bid/ask


            }
        };
    }//namespace static_allocated
}  // namespace esl::economics::markets::order_book

#endif  // ME_ORDER_BOOK_HPP
