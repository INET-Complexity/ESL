/// \file   test_exchange.cpp
///
/// \brief
///
/// \authors    maarten
/// \date       2021-09-28
/// \copyright  Copyright 2017-2021 The Institute for New Economic Thinking,
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
#define BOOST_TEST_MODULE exchange

//#include <boost/test/included/unit_test.hpp>


#include <iostream>
#include <chrono>

#include <boost/program_options.hpp>
using namespace boost::program_options;

#include <esl/data/log.hpp>
#include <esl/economics/currencies.hpp>
#include <esl/economics/markets/order_book/static_order_book.hpp>

#include <esl/economics/markets/quote.hpp>
using namespace esl;
using namespace esl::economics;
using namespace esl::economics::markets;
using namespace esl::economics::markets::order_book;
using esl::economics::markets::order_book::limit_order;
using namespace esl::economics::markets::order_book;


#include <esl/simulation/model.hpp>

//  The types of messages sent by the exchange limit order book are
//
//
//

class twoparamagent
: public agent
{
public:

    simulation::time_point act(simulation::time_interval step, std::seed_seq &seed) override
    {

    }


};


limit_order create(double p, size_t q = 1000, limit_order::side_t side = limit_order::side_t::sell)
{
    esl::economics::markets::ticker ticker_dummy_;
    identity<agent> owner_dummy_;

    auto bid_ = limit_order
            (ticker_dummy_
              , owner_dummy_
              , side
              , quote(price(p, currencies::USD), 100 *  currencies::USD.denominator)
              , q
              , limit_order::lifetime_t::good_until_cancelled
              );

    return bid_;
}

//BOOST_AUTO_TEST_SUITE(ESL)

//BOOST_AUTO_TEST_CASE(exchange_main)






int main(int argument_count, char *arguments[])
{
    //int argument_count = 0;
    //char *arguments[] = {};

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.setf(std::ios::showpoint);

    for(auto a = 0; a < argument_count; ++a) {
        LOG(notice) << arguments[a] << std::endl;
    }

    options_description description_("Allowed options");
    description_.add_options()
    ("help", "produce help message")
    ("stocks", value<unsigned int>()->default_value(1), "set the number of stocks")
    ;

    variables_map arguments_;
    store(parse_command_line(argument_count, arguments, description_), arguments_);

    notify(arguments_);

    if (arguments_.count("help")) {
        std::cout << description_ << std::endl;
        return 0;
    }

    unsigned int stocks_count_;
    if (arguments_.count("stocks")) {
        stocks_count_ = arguments_["stocks"].as<unsigned int>();
    }

    LOG(notice) << "stocks_count_" << stocks_count_ << std::endl;

    auto  min_ = quote(price( 80.00, currencies::USD), 100 *  currencies::USD.denominator);
    auto  max_ = quote(price(120.00, currencies::USD), 100 *  currencies::USD.denominator);
    auto book_ = new static_order_book(min_, max_, 1024*1024);


    std::vector<limit_order> messages_;

    std::minstd_rand generator_;
    std::normal_distribution<> order_limit_(100.0, 1.5);

    std::exponential_distribution<double> order_size_(5.0);

    for(size_t i = 0; i < 1'000; ++i){
        auto limit_ = order_limit_(generator_);
        auto size_ = static_cast<std::uint32_t>(10000 * (order_size_(generator_)) );

        if(0 == generator_() % 5 ){
            // cancel random order
        }

        if(generator_() & 0x1){
            messages_.emplace_back(create(limit_, size_, limit_order::side_t::sell));
        }else{
            messages_.emplace_back(create(limit_, size_, limit_order::side_t::buy));
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    for(const auto &o: messages_){
        book_->insert(o);
        book_->reports.clear();
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

    std::cout << messages_.size() << " orders in " << time_span.count() << " seconds.";
    std::cout << std::endl;


    //book_.reports.clear();
    book_->display(5);

    std::cout << book_->pool_.size() << std::endl;

}


//BOOST_AUTO_TEST_SUITE_END()  // ESL
