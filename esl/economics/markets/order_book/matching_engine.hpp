/// \file   matching_engine.h
///
/// \brief
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

#include <esl/economics/markets/order_book/book.hpp>


namespace esl::economics::markets::order_book {


    class matching_engine
    {
    public:

        typedef std::map<ticker, book> books_t;

        books_t books;

        ///
        ///
        ///
        /// \param order
        /// \return
        bool insert(const order &order)
        {
            auto i = books.find(order.m_symbol);


            if(i == books.end()) {
                i = books.emplace(order.m_symbol, book()).first;
                //i = books.insert(std::make_pair(order.m_symbol, book()))
                //        .first;
            }

            return i->second.insert(order);
        }

        void erase(const order &order)
        {
            auto i = books.find(order.m_symbol);
            if(books.end() != i) {
                i->second.erase(order);
            }
        }

        ///
        /// \param symbol
        /// \param side
        /// \param id
        /// \return
        order &find(const ticker& symbol, order::side_t side, const esl::identity<esl::law::property>& identifier)
        {
            auto i = books.find(symbol);
            if(books.end() == i) {
                // todo: esl exception
                throw std::exception();
            }
            return i->second.find(side, identifier);
        }

        bool match(const ticker& symbol, std::queue<order> &orders)
        {
            auto i = books.find(symbol);
            if(i == books.end()) {
                return false;
            }
            return i->second.match(orders);
        }

        bool match(std::queue<order> &orders)
        {
            for(auto & book : books) {
                book.second.match(orders);
            }
            return !orders.empty();
        }

    };

}  // namespace esl::economics::markets::order_book

#endif  // ME_MATCHING_ENGINE_HPP
