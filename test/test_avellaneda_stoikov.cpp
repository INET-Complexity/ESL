/// \file   test_avellaneda_stoikov.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-06-24
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
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


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE avellaneda_stoikov

#include <boost/test/included/unit_test.hpp>

#include <esl/economics/markets/order_book/avellaneda_stoikov.hpp>
using namespace esl::economics::markets::order_book;

#include <esl/economics/currencies.hpp>

///
/// \detail Use strict comparison because the chosen vanlues are far enough
///         apart
///
BOOST_AUTO_TEST_SUITE(ESL)

    BOOST_AUTO_TEST_CASE(bid_ask)
    {

        for(auto m: std::vector<double>{0.01, 1.00, 100.00, 1'000'000'000.00}){

            auto ba_ = esl::economics::markets::order_book::avellaneda_stoikov( esl::economics::price(m, esl::economics::currencies::USD)
                , esl::quantity(0)
                , esl::quantity(0)
                , 0.
                , 0.20 * m
                , 0.01
                , 1.5 );

            BOOST_CHECK_LT(ba_.first, ba_.second);
        }
    }


    BOOST_AUTO_TEST_CASE(volatility)
    {
        auto middle_ = esl::economics::price(100.00, esl::economics::currencies::USD);


        auto low_ = esl::economics::markets::order_book::avellaneda_stoikov( middle_
            , esl::quantity(0)
            , esl::quantity(0)
            , 0.
            , 0.10 * double(middle_)       // <----
            , 0.01
            , 1.5 );

        auto high_ = esl::economics::markets::order_book::avellaneda_stoikov( middle_
            , esl::quantity(0)
            , esl::quantity(0)
            , 0.
            , 0.30 * double(middle_)      // <----
            , 0.01
            , 1.5 );

        BOOST_CHECK_GT(low_.first, high_.first);
        BOOST_CHECK_LT(low_.second, high_.second);
    }

    BOOST_AUTO_TEST_CASE(risk_aversion)
    {
        auto middle_ = esl::economics::price(100.00, esl::economics::currencies::USD);


        auto low_ = esl::economics::markets::order_book::avellaneda_stoikov( middle_
            , esl::quantity(0)
            , esl::quantity(0)
            , 0.
            , 0.20 * double(middle_)
            , 0.01                                 // <----
            , 1.5 );

        auto high_ = esl::economics::markets::order_book::avellaneda_stoikov( middle_
            , esl::quantity(0)
            , esl::quantity(0)
            , 0.
            , 0.20 * double(middle_)
            , 0.1                                 // <----
            , 1.5 );

        BOOST_CHECK_GT(low_.first, high_.first);
        BOOST_CHECK_LT(low_.second, high_.second);
    }



    BOOST_AUTO_TEST_CASE(end_of_session)
    {
        auto middle_ = esl::economics::price( 100.00
                                            , esl::economics::currencies::USD);


        auto low_ = esl::economics::markets::order_book::avellaneda_stoikov( middle_
            , esl::quantity(0)
            , esl::quantity(0)
            , 0.1                         // <----
            , 0.20 * double(middle_)
            , 0.01
            , 1.5 );

        auto high_ = esl::economics::markets::order_book::avellaneda_stoikov( middle_
            , esl::quantity(0)
            , esl::quantity(0)
            , 0.9                         // <----
            , 0.20 * double(middle_)
            , 0.01
            , 1.5 );

        BOOST_CHECK_LT(low_.first, high_.first);
        BOOST_CHECK_GT(low_.second, high_.second);
    }


BOOST_AUTO_TEST_SUITE_END()  // ESL
