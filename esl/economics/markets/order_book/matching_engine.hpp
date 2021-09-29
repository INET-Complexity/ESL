/// \file   matching_engine.h
///
/// \brief  A matching engine matches order messages with tickers, and directs
///         the matching process in the order book
///
/// \authors    Maarten P. Scholl
/// \date       2020-03-27
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


#ifndef ME_MATCHING_ENGINE_HPP
#define ME_MATCHING_ENGINE_HPP

#include <iostream>
#include <map>


#include <esl/economics/markets/order_book/binary_tree_order_book.hpp>
#include <esl/economics/markets/order_book/order.hpp>
#include <esl/economics/markets/ticker.hpp>
#include <queue>


namespace esl::economics::markets::order_book {

    class matching_engine
    {
    protected:
        std::function<std::shared_ptr<basic_book>(void)> order_book_factory_;
    public:
        std::map<ticker, std::shared_ptr<basic_book> > books;

        explicit matching_engine(std::function<std::shared_ptr<basic_book>(void)> order_book_factory =
                                     [](){
                                         return std::make_shared<binary_tree_order_book>();
                                     })
        : order_book_factory_(order_book_factory)
        {

        }

        ///
        /// \brief
        ///
        /// \param order
        /// \return
        void insert(const limit_order &order)
        {
            auto i = books.find(order.symbol);

            if(i == books.end()) {
                i = books.emplace(order.symbol, order_book_factory_()).first;
            }
            return i->second->insert(order);
        }

        void cancel(const ticker& symbol, const basic_book::order_identifier identifier)
        {
            auto i = books.find(symbol);
            if(books.end() != i){
                i->second->cancel(identifier);
            }
        }

        /*
        ///
        /// \param symbol
        /// \param side
        /// \param id
        /// \return
        limit_order &find( const ticker& symbol
                   , limit_order::side_t side
                   , const esl::identity<esl::law::property>& identifier)
        {
            auto i = books.find(symbol);
            if(books.end() == i) {
                throw std::exception();
            }
            return i->second->find(side, identifier);
        }


        bool match(const ticker &symbol, std::queue<limit_order> &orders)
        {
            auto i = books.find(symbol);
            if(i == books.end()) {
                return false;
            }


            return i->second->insert(orders);
        }

        bool match(std::queue<limit_order> &orders)
        {
            for(auto & book : books) {
                book.second->insert(orders);
            }
            return !orders.empty();
        }
        */
    };

}  // namespace esl::economics::markets::order_book

#endif  // ME_MATCHING_ENGINE_HPP
