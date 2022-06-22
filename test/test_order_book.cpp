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
#include <set>
#include <cmath>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE order_book


#include <boost/test/included/unit_test.hpp>

#define private public
#define protected public
#include <esl/economics/currencies.hpp>
#include <esl/economics/markets/order_book/binary_tree_order_book.hpp>
#include <esl/economics/markets/order_book/static_order_book.hpp>
#undef private
#undef protected
#include <esl/data/representation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/markets/centralized_exchange.hpp>
#include <esl/computation/environment.hpp>
#include <esl/simulation/model.hpp>
#include <esl/economics/markets/quote.hpp>
#include <esl/economics/company.hpp>
#include <esl/interaction/communication.hpp>

using namespace esl;
using namespace esl::economics;
using namespace esl::economics::markets::order_book;
using esl::economics::markets::quote;
using esl::economics::markets::order_book::limit_order;

using namespace esl::economics::markets;

using namespace esl::computation;
using namespace esl::economics;
using namespace esl::economics::finance;
using namespace esl::law;
using namespace esl::economics::currencies;
using namespace esl::simulation;
using namespace esl::simulation::parameter;











BOOST_AUTO_TEST_SUITE(ESL)
    ///
    /// \brief This tests whether memory is initialized correctly
    ///
    BOOST_AUTO_TEST_CASE(statically_allocated_book_construction)
    {
        auto  min_ = quote(price::approximate(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = quote(price::approximate(1'0.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::static_order_book(min_, max_);

        BOOST_CHECK_EQUAL(book_.ticks,10'000);

        BOOST_CHECK_EQUAL(book_.limits_.size(),999 * 100 + 1);

        markets::order_book::static_order_book::limit l_;

        book_.default_encode(book_.valid_limits, quote(min_), l_);
        BOOST_CHECK_EQUAL(l_,0);

        markets::order_book::static_order_book::limit u_;

        book_.default_encode(book_.valid_limits, quote(max_), u_);
        BOOST_CHECK_EQUAL(u_,999 * 100 + 1);
    }

    ///
    /// \brief Here we tests whether incorrect orders are rejected
    ///         as they should be
    ///
    BOOST_AUTO_TEST_CASE(statically_allocated_book_invalidated)
    {
        auto  min_ = quote(price::approximate(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = quote(price::approximate(1'0.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::static_order_book(min_, max_);

        esl::economics::markets::ticker ticker_dummy_;
        identity<agent> owner_dummy_;

        auto bid_toosmall_ = limit_order
            (ticker_dummy_
                , owner_dummy_
                , limit_order::side_t::buy
                , quote(price::approximate(0.001, currencies::USD), 100 *  currencies::USD.denominator)
                , 1000
                , limit_order::lifetime_t::good_until_cancelled
            );

        book_.insert(bid_toosmall_);


        BOOST_CHECK_EQUAL(book_.reports.size(), 1);
        BOOST_CHECK_EQUAL(book_.reports.front().state,  execution_report::invalid);

        auto bid_toolarge_ = limit_order
            (ticker_dummy_
                , owner_dummy_
                , limit_order::side_t::buy
                , quote(price::approximate(10.01, currencies::USD), 100 *  currencies::USD.denominator)
                , 1000
                , limit_order::lifetime_t::good_until_cancelled
            );
        book_.insert(bid_toolarge_);

        BOOST_CHECK_EQUAL(book_.reports.size(), 2);
        BOOST_CHECK_EQUAL(book_.reports.back().state,  execution_report::invalid);
    }


    limit_order create_bid(double p, size_t q = 1000)
    {
        esl::economics::markets::ticker ticker_dummy_;
        identity<agent> owner_dummy_;

        auto bid_ = limit_order
            (ticker_dummy_
                , owner_dummy_
                , limit_order::side_t::buy
                , quote(price::approximate(p, currencies::USD), 100 *  currencies::USD.denominator)
                , q
                , limit_order::lifetime_t::good_until_cancelled
            );

        return bid_;
    }


    limit_order create_ask(double p, size_t q = 1000)
    {
        esl::economics::markets::ticker ticker_dummy_;
        identity<agent> owner_dummy_;

        auto bid_ = limit_order
            (ticker_dummy_
                , owner_dummy_
                , limit_order::side_t::sell
                , quote(price::approximate(p, currencies::USD), 100 *  currencies::USD.denominator)
                , q
                , limit_order::lifetime_t::good_until_cancelled
            );

        return bid_;
    }

    ///
    /// \brief  This test is to see whether the function that maps quotes of
    ///         any type to an array index yields unique results and that the
    ///         the array index is converted back to the right quote.
    ///
    BOOST_AUTO_TEST_CASE(statically_allocated_book_encode_decode)
    {
        auto  min_ = quote(price::approximate(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = quote(price::approximate(10.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::static_order_book(min_, max_);

        std::set<markets::order_book::static_order_book::limit> unique_;

        for(double p: std::vector<double>{
            0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09,
            4.96, 4.97, 4.98, 4.99, 5.00, 5.01, 5.02, 5.03, 5.04,
            9.92, 9.93, 9.94, 9.95, 9.96, 9.97, 9.98, 9.99, 10.00
        }){
            markets::order_book::static_order_book::limit l;
            // use nextafter, because otherwise we make floating point errors
            auto q = quote(price::approximate(std::nextafter(p, p+0.01), currencies::USD)
                                   , 100 *  currencies::USD.denominator);
            BOOST_CHECK(book_.encode(book_.valid_limits, q, l));
            auto array_index_ = l;

            // testing this seems somewhat redundant, but this can
            // detect a broken comparison operator in the quote class
            // or bad floating point conversions

            BOOST_CHECK(unique_.find(array_index_) == unique_.end());
            unique_.insert(array_index_);

            auto rtq_ = book_.decode(book_.valid_limits, l);    // round trip value
            BOOST_CHECK_EQUAL(q, rtq_);


        }

    }

    BOOST_AUTO_TEST_CASE(statically_allocated_book_placement)
    {
        auto  min_ = quote(price::approximate(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = quote(price::approximate(10.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::static_order_book(min_, max_);

        book_.insert(create_bid(4.75));
        BOOST_CHECK_EQUAL(book_.reports.front().state,  execution_report::placement);

        book_.insert(create_bid(4.75));
        book_.insert(create_bid(4.76));
        book_.insert(create_bid(4.74));
        BOOST_CHECK_EQUAL(book_.reports.size(), 4);
        BOOST_CHECK_EQUAL(book_.reports.back().state,  execution_report::placement);

        BOOST_CHECK(book_.bid());
        BOOST_CHECK_EQUAL(book_.bid().value(), quote(price::approximate(4.76, currencies::USD), 100 *  currencies::USD.denominator));
    }



    BOOST_AUTO_TEST_CASE(statically_allocated_book_match)
    {
        auto  min_ = quote(price::approximate(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = quote(price::approximate(10.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::static_order_book(min_, max_);

        book_.insert(create_bid(4.75, 500));
        book_.insert(create_bid(4.75, 500));
        book_.insert(create_bid(4.74));
        book_.insert(create_bid(4.76, 500));


//        book_.display(); std::cout << std::endl;


        book_.insert(create_ask(4.69, 750));    // expect: execute 500 at 4.76 and remainder of 250 at 4.75, leaving 750

//        book_.display(); std::cout << std::endl;

        BOOST_CHECK_EQUAL(book_.bid().value(), quote(price::approximate(4.75, currencies::USD), 100 *  currencies::USD.denominator));

        book_.insert(create_ask(4.75, 1000));   //  expect: execute 750 at 4.75 and then place 250 in book at 4.75, improving best ask

//        book_.display(); std::cout << std::endl;

        BOOST_CHECK_EQUAL(book_.bid().value(), quote(price::approximate(4.74, currencies::USD), 100 *  currencies::USD.denominator));
        BOOST_CHECK_EQUAL(book_.ask().value(), quote(price::approximate(4.75, currencies::USD), 100 *  currencies::USD.denominator));


        book_.insert(create_bid(4.75, 200));   //  expect: execute 200 at 4.75
        BOOST_CHECK_EQUAL(book_.ask().value(), quote(price::approximate(4.75, currencies::USD), 100 *  currencies::USD.denominator));
        BOOST_CHECK_EQUAL(book_.reports.back().quantity, 200);


//        book_.display(); std::cout << std::endl;

        book_.insert(create_bid(4.75, 51));   //  expect: execute 750 at 4.75 and then place 250 in book at 4.75, improving best ask

//        book_.display(); std::cout << std::endl;

        BOOST_CHECK(!book_.ask());
        BOOST_CHECK_EQUAL(book_.bid().value(), quote(price::approximate(4.75, currencies::USD), 100 *  currencies::USD.denominator));
    }







    BOOST_AUTO_TEST_CASE(statically_allocated_book_match2)
    {
        auto  min_ = quote(price::approximate(0.01, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = quote(price::approximate(10.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = markets::order_book::static_order_book(min_, max_);

        book_.insert(create_bid(5.00, 500));
        book_.insert(create_bid(4.75, 500));
        book_.insert(create_bid(4.74, 1000));
        book_.insert(create_bid(4.76, 500));


//        book_.display(); std::cout << std::endl;

        book_.insert(create_ask(4.69, 750));    // expect: execute 500 at 4.76 and remainder of 250 at 4.75, leaving 750

//        book_.display(); std::cout << std::endl;

        BOOST_CHECK_EQUAL(book_.bid().value(), quote(price::approximate(4.76, currencies::USD), 100 *  currencies::USD.denominator));

        book_.insert(create_ask(4.75, 1000));   //  expect: execute 750 at 4.75 and then place 250 in book at 4.75, improving best ask

//        book_.display(); std::cout << std::endl;

        BOOST_CHECK_EQUAL(book_.bid().value(), quote(price::approximate(4.74, currencies::USD), 100 *  currencies::USD.denominator));
        BOOST_CHECK_EQUAL(book_.ask().value(), quote(price::approximate(4.75, currencies::USD), 100 *  currencies::USD.denominator));


        book_.insert(create_bid(4.75, 200));   //  expect: execute 200 at 4.75
        BOOST_CHECK_EQUAL(book_.ask().value(), quote(price::approximate(4.75, currencies::USD), 100 *  currencies::USD.denominator));
        BOOST_CHECK_EQUAL(book_.reports.back().quantity, 200);

//        book_.display(); std::cout << std::endl;

        book_.insert(create_bid(4.75, 51));
//        book_.display(); std::cout << std::endl;

        BOOST_CHECK(!book_.ask());
        BOOST_CHECK_EQUAL(book_.bid().value(), quote(price::approximate(4.75, currencies::USD), 100 *  currencies::USD.denominator));
    }

    limit_order create(double p, size_t q = 1000, limit_order::side_t side = limit_order::side_t::sell)
    {
        esl::economics::markets::ticker ticker_dummy_;
        identity<agent> owner_dummy_;

        auto bid_ = limit_order
            (ticker_dummy_
                , owner_dummy_
                , side
                , quote(price::approximate(p, currencies::USD), 100 *  currencies::USD.denominator)
                , q
                , limit_order::lifetime_t::good_until_cancelled
            );

        return bid_;
    }

#ifdef ESL_RELEASE
    BOOST_AUTO_TEST_CASE(statically_allocated_book_performance)
    {
        auto  min_ = quote(price::approximate( 80.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto  max_ = quote(price::approximate(120.00, currencies::USD), 100 *  currencies::USD.denominator);
        auto book_ = new static_order_book(min_, max_, 1024*1024);

        std::vector<limit_order> messages_;

        std::minstd_rand generator_;
        std::normal_distribution<> order_limit_(100.0, 1.5);

        std::exponential_distribution<double> order_size_(5.0);

        for(size_t i = 0; i < 1000'000; ++i){
            auto limit_ = order_limit_(generator_);
            auto size_ = static_cast<std::uint32_t>(10000 * (order_size_(generator_)) );


            if(generator_() & 0x1){
                messages_.emplace_back(create(limit_, size_, limit_order::side_t::sell));
            }else{
                messages_.emplace_back(create(limit_, size_, limit_order::side_t::buy));
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

//        book_->display(5);

        std::cout << book_->pool_.size() << std::endl;
    }
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr time_point market_open_  = time_point(( 9 * 60 * 60 + 30 * 60) * std::nano::den);
constexpr time_point market_close_ = time_point( 16 * 60 * 60 * std::nano::den);


class trader
: public agent
, public identifiable_as<trader>
{
private:
public:
    double inter_wakeup_time_seconds = 120;

    interaction::communication::shifted_gamma_latency network_model;

    struct market_metadata
    {
        identity<market> exchange;
        time_point open;
        time_point close;
        std::vector<ticker> traded;
    };

    std::vector<market_metadata> markets;

    // TODO: non-general
    price fundamental_estimate;

    time_point next_wakeup = std::numeric_limits<time_point>::max();


    explicit trader(const identity<trader> &i)
    : agent(i)
    , network_model(2.5, 0.1 * std::nano::den, 0.01 * std::nano::den)
    {

    }

    ///
    /// \brief
    ///
    void execution_strategy( simulation::time_interval step
                           , std::seed_seq &seed
                           , const ticker &t
                           , const identity<market> &m
                           )
    {
        limit_order o;
        o.symbol = t;

        o.owner = identifier;
        o.lifetime = order_book::limit_order::lifetime_t::good_until_cancelled;

        std::binomial_distribution<size_t> binomial_(1000, 0.5);

        std::mt19937 generator_(seed);
        o.side     = (generator_() % 2 == 0 ? limit_order::buy : limit_order::sell);
        o.quantity = binomial_(generator_);
        o.limit    = quote(fundamental_estimate, USD.denominator );

        auto latency = network_model.sample(identifier, m, seed);
        auto message = this->template create_message<new_order_single>
                ( m
                  , step.lower + latency
                  );
        message->sent = step.lower;
        message->order_details = o;
    }

    time_duration random_sleep(std::seed_seq &seed)
    {
        std::exponential_distribution exponential_distribution_(1.0 / (inter_wakeup_time_seconds * std::nano::den) );
        std::mt19937 genenerator_(seed);
        auto real_ = exponential_distribution_(genenerator_);
        return time_duration(real_);

    }

    ///
    /// \brief
    ///
    time_point act(simulation::time_interval step, std::seed_seq &seed) override
    {



        bool open_market_ = false;
        time_point first_market_open_ = step.upper;
        for(const auto &m: markets){
            if(m.open > step.lower){
                first_market_open_ = std::min(first_market_open_, m.open);
            }

            if(m.open <= step.lower &&  step.lower <= m.close){
                open_market_ = true;
                break;
            }
        }

        if(!open_market_){
            next_wakeup = first_market_open_ + random_sleep(seed);
            return next_wakeup;
        }

        if(step.lower < next_wakeup){
            return next_wakeup;
        }

        for(const auto &m: markets){
            for(const auto &t: m.traded){
                execution_strategy(step, seed, t, m.exchange);
            }
        }

        next_wakeup = step.lower + random_sleep(seed);
        if(next_wakeup >= market_close_){
            next_wakeup = step.upper;
            return step.upper;
        }
        return next_wakeup;
    }
};



BOOST_AUTO_TEST_CASE(chiarella_iori_2002)
{
//    std::cout << "sizeof(entity<>) = " << sizeof(simulation::entity<agent>) << std::endl;

    std::cout << "sizeof(communicator) = " << sizeof(interaction::communicator) << std::endl;

    std::cout << "sizeof(agent) = " << sizeof(agent) << std::endl;

    std::cout << "sizeof(trader) = " << sizeof(trader) << std::endl;


    size_t lot_size = 1;

    auto  min_ = markets::quote(price(    0.01, USD), lot_size * USD.denominator);
    auto  max_ = markets::quote(price(10000.00, USD), lot_size * USD.denominator);

    // define the maximum number of open orders
    // as the static_order_book maintains a fixed-size memory pool
    constexpr size_t max_orders_ = 1024*1024;

    // creates the order book
    auto book_ = std::make_shared<static_order_book>(min_, max_, max_orders_);

    size_t sample = 1;

    environment e;
    model model_(e, parametrization(0, 0, 24 * 60 * 60 * std::nano::den));


    markets::ticker ticker_({0,1}, {0,2});
    std::vector<markets::ticker> traded_ = {ticker_};
    auto exchange_ = model_.template create<centralized_exchange>(model_.world, traded_);
    exchange_->books.emplace(ticker_, book_);

    size_t agents = 10;
    for(size_t a = 1; a < 1 + agents; ++a){
        centralized_exchange::position_report position_;
        position_.supply = 1'000;

        auto t = model_.template create<trader>(model_.world);

        // TODO: non-general
        std::seed_seq seed_ = {a};
        std::mt19937 generator_(seed_);
        std::lognormal_distribution<double> normal_(std::log(10.), 0.1);
        t->fundamental_estimate = price::approximate(normal_(generator_), USD);

        t->markets.push_back( {(*exchange_), market_open_, market_close_, {ticker_}}  );

        exchange_->positions[ticker_].emplace(t->identifier, position_);
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    e.run(model_);

    auto t2 = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

    std::cout << agents << " traded in " << time_span.count() << " seconds.";
    std::cout << std::endl;

    book_->display();
}



BOOST_AUTO_TEST_SUITE_END()  // ESL
