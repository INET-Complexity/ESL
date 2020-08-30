/// \file   test_order_book.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-06-27
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



#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE order_book


#include <boost/test/included/unit_test.hpp>

#define private public
#define protected public
#include <esl/economics/currencies.hpp>
#include <esl/economics/markets/order_book/book.hpp>
#undef private
#undef protected
#include <esl/data/representation.hpp>

using namespace esl;
using namespace esl::economics;
using namespace esl::economics::markets::order_book;
using esl::economics::markets::order_book::limit_order_message;
using namespace esl::economics::markets::order_book::statically_allocated;



BOOST_AUTO_TEST_SUITE(ESL)

    ///
    /// \brief This tests whether memory is initialized correctly
    ///
    BOOST_AUTO_TEST_CASE(statically_allocated_book_construction)
    {
        auto  min_ = markets::quote(price(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = markets::quote(price(1'0.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::statically_allocated::book<10>(min_, max_);

        BOOST_CHECK_EQUAL(book_.ticks,10'000);

        BOOST_CHECK_EQUAL(book_.limits_.size(),999 * 100 + 1);

        markets::order_book::statically_allocated::book<10>::limit l_;

        book_.default_encode(markets::quote(min_), l_);
        BOOST_CHECK_EQUAL(l_,0);

        markets::order_book::statically_allocated::book<10>::limit u_;

        book_.default_encode(markets::quote(max_), u_);
        BOOST_CHECK_EQUAL(u_,999 * 100 + 1);
    }

    ///
    /// \brief Here we tests whether incorrect orders are rejected
    ///         as they should be
    ///
    BOOST_AUTO_TEST_CASE(statically_allocated_book_invalidated)
    {
        auto  min_ = markets::quote(price(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = markets::quote(price(1'0.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::statically_allocated::book<10>(min_, max_);

        esl::economics::markets::ticker ticker_dummy_;
        identity<agent> owner_dummy_;

        auto bid_toosmall_ = limit_order_message
        ( ticker_dummy_
        , owner_dummy_
        , limit_order_message::side_t::buy
        , markets::quote(price(0.001, currencies::USD), 100 *  currencies::USD.denominator)
        , 1000
        ,limit_order_message::lifetime_t::good_until_cancelled
        );

        book_.insert(bid_toosmall_);


        BOOST_CHECK_EQUAL(book_.reports.size(), 1);
        BOOST_CHECK_EQUAL(book_.reports.front().state,  execution_report::invalid);

        auto bid_toolarge_ = limit_order_message
        ( ticker_dummy_
        , owner_dummy_
        , limit_order_message::side_t::buy
        , markets::quote(price(10.01, currencies::USD), 100 *  currencies::USD.denominator)
        , 1000
        ,limit_order_message::lifetime_t::good_until_cancelled
        );
        book_.insert(bid_toolarge_);

        BOOST_CHECK_EQUAL(book_.reports.size(), 2);
        BOOST_CHECK_EQUAL(book_.reports.back().state,  execution_report::invalid);
    }


    limit_order_message create_bid(double p, size_t q = 1000)
    {
        esl::economics::markets::ticker ticker_dummy_;
        identity<agent> owner_dummy_;

        auto bid_ = limit_order_message
            ( ticker_dummy_
                , owner_dummy_
                , limit_order_message::side_t::buy
                , markets::quote(price(p, currencies::USD), 100 *  currencies::USD.denominator)
                , q
                ,limit_order_message::lifetime_t::good_until_cancelled
            );

        return bid_;
    }


    limit_order_message create_ask(double p, size_t q = 1000)
    {
        esl::economics::markets::ticker ticker_dummy_;
        identity<agent> owner_dummy_;

        auto bid_ = limit_order_message
            ( ticker_dummy_
                , owner_dummy_
                , limit_order_message::side_t::sell
                , markets::quote(price(p, currencies::USD), 100 *  currencies::USD.denominator)
                , q
                ,limit_order_message::lifetime_t::good_until_cancelled
            );

        return bid_;
    }

    ///
    /// \brief  This test is to see
    ///
    BOOST_AUTO_TEST_CASE(statically_allocated_book_encode_decode)
    {
        auto  min_ = markets::quote(price(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = markets::quote(price(10.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::statically_allocated::book<20>(min_, max_);

        for(double p: std::vector<double>{
                0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09,
                4.96, 4.97, 4.98, 4.99, 5.00, 5.01, 5.02, 5.03, 5.04,
                9.92, 9.93, 9.94, 9.95, 9.96, 9.97, 9.98, 9.99, 10.00
            }){
            markets::order_book::statically_allocated::book<10>::limit l;
            auto q = markets::quote(price(p, currencies::USD), 100 *  currencies::USD.denominator);
            book_.encode(q, l);
            auto rtq_ = book_.decode(l);    // round trip value
            BOOST_CHECK_EQUAL(q, rtq_);
        }

    }

    BOOST_AUTO_TEST_CASE(statically_allocated_book_placement)
    {
        auto  min_ = markets::quote(price(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = markets::quote(price(10.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::statically_allocated::book<10>(min_, max_);

        book_.insert(create_bid(4.75));
        BOOST_CHECK_EQUAL(book_.reports.front().state,  execution_report::placement);

        book_.insert(create_bid(4.75));
        book_.insert(create_bid(4.76));
        book_.insert(create_bid(4.74));
        BOOST_CHECK_EQUAL(book_.reports.size(), 4);
        BOOST_CHECK_EQUAL(book_.reports.back().state,  execution_report::placement);

        BOOST_CHECK(book_.bid());
        BOOST_CHECK_EQUAL(book_.bid().value(), markets::quote(price(4.76, currencies::USD), 100 *  currencies::USD.denominator));
    }



    BOOST_AUTO_TEST_CASE(statically_allocated_book_match)
    {
        auto  min_ = markets::quote(price(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = markets::quote(price(10.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::statically_allocated::book<10>(min_, max_);

        book_.insert(create_bid(4.75, 500));
        book_.insert(create_bid(4.75, 500));
        book_.insert(create_bid(4.74));
        book_.insert(create_bid(4.76, 500));


        book_.display(); std::cout << std::endl;


        book_.insert(create_ask(4.69, 750));    // expect: execute 500 at 4.76 and remainder of 250 at 4.75, leaving 750

        book_.display(); std::cout << std::endl;

        BOOST_CHECK_EQUAL(book_.bid().value(), markets::quote(price(4.75, currencies::USD), 100 *  currencies::USD.denominator));

        book_.insert(create_ask(4.75, 1000));   //  expect: execute 750 at 4.75 and then place 250 in book at 4.75, improving best ask

        book_.display(); std::cout << std::endl;

        BOOST_CHECK_EQUAL(book_.bid().value(), markets::quote(price(4.74, currencies::USD), 100 *  currencies::USD.denominator));
        BOOST_CHECK_EQUAL(book_.ask().value(), markets::quote(price(4.75, currencies::USD), 100 *  currencies::USD.denominator));


        book_.insert(create_bid(4.75, 200));   //  expect: execute 200 at 4.75
        BOOST_CHECK_EQUAL(book_.ask().value(), markets::quote(price(4.75, currencies::USD), 100 *  currencies::USD.denominator));
        BOOST_CHECK_EQUAL(book_.reports.back().quantity, 200);


        book_.display(); std::cout << std::endl;

        book_.insert(create_bid(4.75, 51));   //  expect: execute 750 at 4.75 and then place 250 in book at 4.75, improving best ask

        book_.display(); std::cout << std::endl;

        BOOST_CHECK(!book_.ask());
        BOOST_CHECK_EQUAL(book_.bid().value(), markets::quote(price(4.75, currencies::USD), 100 *  currencies::USD.denominator));
    }

    limit_order_message create(double p, size_t q = 1000, limit_order_message::side_t side = limit_order_message::side_t::sell)
    {
        esl::economics::markets::ticker ticker_dummy_;
        identity<agent> owner_dummy_;

        auto bid_ = limit_order_message
            ( ticker_dummy_
                , owner_dummy_
                , side
                , markets::quote(price(p, currencies::USD), 100 *  currencies::USD.denominator)
                , q
                ,limit_order_message::lifetime_t::good_until_cancelled
            );

        return bid_;
    }

#if ESL_RELEASE == 1
    BOOST_AUTO_TEST_CASE(statically_allocated_book_performance)
    {
        auto  min_ = quote(price( 80.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = quote(price(120.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = new book<1024*1024>(min_, max_);

        std::vector<limit_order_message> messages_;

        std::minstd_rand generator_;
        std::normal_distribution<> order_limit_(100.0, 1.5);

        std::exponential_distribution<double> order_size_(5.0);

        for(size_t i = 0; i < 1000'000; ++i){
            auto limit_ = order_limit_(generator_);
            auto size_ = static_cast<std::uint32_t>(10000 * (order_size_(generator_)) );


            if(generator_() & 0x1){
                messages_.emplace_back(create(limit_, size_, limit_order_message::side_t::sell));
            }else{
                messages_.emplace_back(create(limit_, size_, limit_order_message::side_t::buy));
            }
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        for(const auto &o: messages_){

            if(0 == generator_() % 5 ){
                // cancel random order
            }

            book_->insert(o);
            book_->reports.clear();
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

        std::cout << messages_.size() << " orders in " << time_span.count() << " seconds.";
        std::cout << std::endl;

        book_->display(5);

        std::cout << book_->pool_.size() << std::endl;
    }
#endif

BOOST_AUTO_TEST_SUITE_END()  // ESL
