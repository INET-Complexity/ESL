/// \file   order_book.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2021-11-01
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


// import the order book
#include <esl/economics/markets/centralized_exchange.hpp>

// import currencies, because we will be trading the stock against USD
#include <esl/economics/currencies.hpp>

using namespace esl;
using namespace esl::economics;


//
int main(int argc, char** argv) {

    auto  min_ = markets::quote(price( 0.01, currencies::USD), lot_size * currencies::USD.denominator);
    auto  max_ = markets::quote(price(10.00, currencies::USD), lot_size * currencies::USD.denominator);

    // define the maximum number of open orders
    // as the static_order_book maintains a fixed-size memory pool
    constexpr size_t max_orders_ = 1'00'000;

    // creates the order book
    auto book_ = static_order_book(min_, max_, 1024*1024);

    book_.insert(create(4.75, 500, limit_order_message::side_t::buy));
    book_.insert(create(4.75, 750, limit_order_message::side_t::sell));

}