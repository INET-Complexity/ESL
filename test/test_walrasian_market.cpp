/// \file   test_walrasian_market.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-08
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
#include <tuple>
#include <utility>


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE walrasian_market

#include <boost/test/included/unit_test.hpp>

#include <esl/computation/environment.hpp>
#include <esl/data/log.hpp>
#include <esl/economics/company.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
#include <esl/law/jurisdictions.hpp>
#include <esl/simulation/model.hpp>

#include <esl/economics/markets/walras/quote_message.hpp>

using namespace esl;
using namespace esl::economics;
using namespace esl::economics::markets;
using namespace esl::economics::finance;
using namespace esl::economics::markets::walras;
using namespace esl::law;
using esl::economics::company;

using std::map;
using std::vector;


///
/// \brief  A simple test trader order with constant wealth allocation
///
struct test_trader_order
: public differentiable_order_message
{
    ///
    /// \brief  Total wealth of the agent to be distributed
    ///
    double capital;

    ///
    /// \brief  Allocation fractions
    ///
    map<identity<law::property>, double> allocation;

    ///
    /// \brief  Supply of long positions and securities lending contracts
    ///
    //map<identity<law::property>, quantity> supply;

    // inherit base class constructor
   // using differentiable_order_message::differentiable_order_message;

    ///
    /// \param capital
    /// \param allocation
    /// \param supply
    /// \param q
    test_trader_order( double capital
                     , map<identity<law::property>, double> allocation
                     , const walras::quote_message &q = walras::quote_message())

    : differentiable_order_message(q.sender, q.recipient)
    , capital(capital)
    , allocation(move(allocation))
    {

    }

    ///
    /// \brief  computes excess demand for each property individually
    /// \param quotes   for each property id, a tuple of the quote and differentiable scalar
    /// \return
    map<identity<law::property>, variable> excess_demand(const map<identity<law::property>, std::tuple<quote, variable>> &quotes)
    const override
    {
        map<identity<law::property>, variable> excess_demand_;
        for(const auto &[k, v]: allocation) {
            auto quote_price_ = static_cast<double>(std::get<0>(quotes.find(k)->second));
            double supply_ = 0;
            auto iterator_ = supply.find(k);
            if(supply.end() != iterator_) {
                supply_ = double(std::get<0>(iterator_->second) - std::get<1>(iterator_->second));
            }
            excess_demand_.insert({k, (v * capital) - supply_ * (quote_price_ * std::get<1>(quotes.find(k)->second))});
        }
        return excess_demand_;
    }
};


struct test_constant_demand_trader
: public shareholder
{
    // set trader wealth for this test
    double wealth = 1000.00;

    test_constant_demand_trader(const identity<shareholder> &i)
    : agent(i)
    , economics::finance::shareholder(i)
    {

    }

    simulation::time_point act( simulation::time_interval step
                              , std::seed_seq &seed
                              ) override
    {
        (void) seed;
        for(auto [k, message_]: inbox) {
            switch(message_->type){
            case walras::quote_message::code:
                auto quote_ = std::dynamic_pointer_cast<walras::quote_message>(message_);
                map<identity<property>, double> allocation;
                size_t assets_ = quote_->proposed.size();
                size_t denominator_ =  (assets_ * (1 + assets_))/2;
                size_t a = 1;
                for(auto [k, q]: quote_->proposed){
                    allocation.emplace(k->identifier,  double(a) / denominator_ );
                    ++a;
                }

                // make an order message
                auto message_ = this->template create_message<test_trader_order>( quote_->sender
                                                                                , step.lower
                                                                                , wealth
                                                                                , allocation
                                                                                , *quote_
                                                                                 );
                message_->sent = step.lower;
                for(auto [k, q]: quote_->proposed){
                    message_->supply.emplace(k->identifier, std::make_tuple(500, quantity(0)));
                }
            }
        }
        return step.upper;
    }
};


BOOST_AUTO_TEST_SUITE(ESL)

///
/// \brief  Tests that a market quotes the right number of prices for property,
///         and that these are delivered to participants.
///
BOOST_AUTO_TEST_CASE(walras_market_quote_multiple_assets)
{
    computation::environment environment_;
    simulation::model model_(environment_, simulation::parameter::parametrization(0, 0, 10));

    vector<std::tuple<std::shared_ptr<company>, share_class>> share_classes_;

    typedef std::tuple<identity<company>, share_class> key_t;
    map<key_t, identity<law::property>> stocks_;

    // set the number of traded assets and create a container
    law::property_map<quote> traded_assets_;
    size_t assets = 2;

    std::vector<std::shared_ptr<property>> properties_;

    // create the assets, by creating the companies that issued the shares first
    for(size_t a = 0; a < assets; ++a) {
        auto company_identifier_ = model_.template create_identifier<company>();

        // create a US company
        auto company_ = std::make_shared<company>(company_identifier_, law::jurisdictions::US);

        // create a share issue for the company for several shares
        auto main_issue_ = share_class();
        company_->shares_outstanding[main_issue_] = 1'000;

        // create stocks from the shares
        for(const auto &[share_, quantity] : company_->shares_outstanding) {
            (void) quantity;
            auto stock_ = std::make_shared<stock>(*company_, share_);

            // suggest to the market that $1.00 is the starting price
            traded_assets_.insert({stock_, quote(price::approximate(1.00, currencies::USD))});
            share_classes_.emplace_back(make_tuple(company_, share_));

            // create a key to store these in associative datastructures
            auto key_ = std::make_tuple<identity<company>, share_class>(*company_, share_class(share_));
            stocks_.insert({key_, stock_->identifier});

            properties_.push_back(stock_);
        }
    }

    auto market_ = model_.template create<price_setter>();
    market_->traded_properties = traded_assets_;

    // create a collection with trading agents who are also shareholders
    vector<std::shared_ptr<test_constant_demand_trader>> participants_ =
        {   model_.template create<test_constant_demand_trader>()
        ,   model_.template create<test_constant_demand_trader>()
        };

    for(auto &p : participants_) {
        std::cout << "test_constant_demand_trader " << p->identifier << std::endl;
        // add the trading agents to the market participant list
        market_->participants.insert((*p));

        // notify agents about the stock and key
        for(const auto &[k, v]: stocks_) {
            p->stocks.insert(std::make_pair(k, v));
        }

        // give agents an endowment of stocks
        for(const auto &[k, v]: traded_assets_) {
            (void)v;
            (*p).shareholder::owner<stock>::properties.insert(std::dynamic_pointer_cast<stock>(k),quantity(500));
        }

        std::cout << "created " << share_classes_.size() << " share classes" << std::endl;

        // notify company of new shareholders
        for(auto &share: share_classes_) {
            auto c = std::get<0>(share);
            map<share_class, std::uint64_t> holdings_;
            holdings_.emplace(std::get<1>(share), 500);
            c->shareholders.emplace(*p, holdings_);
        }
    }

    // during the first timestep, the market sends out quotes, and nothing else happens
    model_.step({0, 1});

    // expect: prices unchanged from initial price given above
    BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[0])->second.type) == price(100, currencies::USD));
    BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[1])->second.type) == price(100, currencies::USD));

    // we have the first market interaction: prices are formed by the market agent
    model_.step({1, 2});

    // prices are updated
    BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[0])->second.type) == price( 66, currencies::USD));
    BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[1])->second.type) == price(133, currencies::USD));

    // the agent does not know the new prices until after one time step
    BOOST_CHECK_EQUAL(participants_[0]->prices.size(), assets);
    BOOST_TEST(participants_[0]->prices.find(properties_[0])->second == price(100, currencies::USD));
    BOOST_TEST(participants_[0]->prices.find(properties_[1])->second == price(100, currencies::USD));
    //log(notice) << participants_[0]->prices << std::endl;

    // the market runs again
    model_.step({2, 3});
    // the agent now knows the previous prices from step [1, 2)
    //log(notice) << participants_[0]->prices << std::endl;
    BOOST_TEST(participants_[0]->prices.find(properties_[0])->second == price( 66, currencies::USD));
    BOOST_TEST(participants_[0]->prices.find(properties_[1])->second == price(133, currencies::USD));

    // the market prices are computed again
    BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[0])->second.type) == price( 66, currencies::USD));
    BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[1])->second.type) == price(133, currencies::USD));
}


///
/// \brief  Tests that a market quotes the right number of prices for property,
///         and that these are delivered to participants.
///
    BOOST_AUTO_TEST_CASE(walras_market_quote_single_asset)
    {
        computation::environment environment_;
        simulation::model model_(environment_, simulation::parameter::parametrization(0, 0, 10));

        vector<std::tuple<std::shared_ptr<company>, share_class>> share_classes_;

        typedef std::tuple<identity<company>, share_class> key_t;
        map<key_t, identity<law::property>> stocks_;

        // set the number of traded assets and create a container
        law::property_map<quote> traded_assets_;
        size_t assets = 1;

        std::vector<std::shared_ptr<property>> properties_;

        // create the assets, by creating the companies that issued the shares first
        for(size_t a = 0; a < assets; ++a) {
            auto company_identifier_ = model_.template create_identifier<company>();

            // create a US company
            auto company_ = std::make_shared<company>(company_identifier_, law::jurisdictions::US);

            // create a share issue for the company for several shares
            auto main_issue_ = share_class();
            company_->shares_outstanding[main_issue_] = 1'000;

            // create stocks from the shares
            for(const auto &[share_, quantity] : company_->shares_outstanding) {
                (void) quantity;
                auto stock_ = std::make_shared<stock>(*company_, share_);

                // suggest to the market that $1.00 is the starting price
                traded_assets_.insert({stock_, quote(price::approximate(1.00, currencies::USD))});
                share_classes_.emplace_back(make_tuple(company_, share_));

                // create a key to store these in associative datastructures
                auto key_ = std::make_tuple<identity<company>, share_class>(*company_, share_class(share_));
                stocks_.insert({key_, stock_->identifier});

                properties_.push_back(stock_);
            }
        }

        auto market_ = model_.template create<price_setter>();
        market_->traded_properties = traded_assets_;

        // create a collection with trading agents who are also shareholders
        vector<std::shared_ptr<test_constant_demand_trader>> participants_ =
            {   model_.template create<test_constant_demand_trader>()
                ,   model_.template create<test_constant_demand_trader>()
            };

        for(auto &p : participants_) {
            std::cout << "test_constant_demand_trader " << p->identifier << std::endl;
            // add the trading agents to the market participant list
            market_->participants.insert((*p));

            // notify agents about the stock and key
            for(const auto &[k, v]: stocks_) {
                p->stocks.insert(std::make_pair(k, v));
            }

            // give agents an endowment of stocks
            for(const auto &[k, v]: traded_assets_) {
                (void)v;
                (*p).shareholder::owner<stock>::properties.insert(std::dynamic_pointer_cast<stock>(k),quantity(500));
            }

            std::cout << "created " << share_classes_.size() << " share classes" << std::endl;

            // notify company of new shareholders
            for(auto &share: share_classes_) {
                auto c = std::get<0>(share);
                map<share_class, std::uint64_t> holdings_;
                holdings_.emplace(std::get<1>(share), 500);
                c->shareholders.emplace(*p, holdings_);
            }
        }

        // during the first timestep, the market sends out quotes, and nothing else happens
        model_.step({0, 1});

        // expect: prices unchanged from initial price given above
        BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[0])->second.type) == price(100, currencies::USD));

        // we have the first market interaction: prices are formed by the market agent
        model_.step({1, 2});

        // prices are updated
        BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[0])->second.type) == price(200, currencies::USD));

        // the agent does not know the new prices until after one time step
        BOOST_CHECK_EQUAL(participants_[0]->prices.size(), assets);
        BOOST_TEST(participants_[0]->prices.find(properties_[0])->second == price(100, currencies::USD));
        //log(notice) << participants_[0]->prices << std::endl;

        // the market runs again
        model_.step({2, 3});
        // the agent now knows the previous prices from step [1, 2)
        //log(notice) << participants_[0]->prices << std::endl;
        BOOST_TEST(participants_[0]->prices.find(properties_[0])->second == price(200, currencies::USD));

        // the market prices are computed again
        BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[0])->second.type) == price(200, currencies::USD));
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL
