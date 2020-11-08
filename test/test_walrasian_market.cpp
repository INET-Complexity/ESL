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

#include <esl/economics/finance/company.hpp>

#include <esl/computation/environment.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
#include <esl/law/jurisdictions.hpp>
#include <esl/simulation/model.hpp>

#include <esl/economics/markets/walras/quote_message.hpp>

using namespace esl;
using namespace esl::economics;
using namespace esl::economics::markets;
using namespace esl::economics::finance;
using esl::economics::company;


struct test_trader_order
: public economics::markets::walras::differentiable_order_message
{
    double capital;
    const std::vector<double> allocation;
    std::map<identity<law::property>, quantity> supply;
    std::map<identity<law::property>, economics::price>
        valuations;

    using economics::markets::walras::differentiable_order_message::
        differentiable_order_message;

    test_trader_order(
        double capital = 0.0, std::vector<double> allocation = {},
        std::map<identity<law::property>, quantity> supply = {},
        std::map<identity<law::property>, economics::price>
            valuations = {}
        ,
        const economics::markets::walras::quote_message &q =
            economics::markets::walras::quote_message())

    : economics::markets::walras::differentiable_order_message(q.sender, q.recipient)
    , capital(capital)
    , allocation(move(allocation))
    , supply(move(supply))
    , valuations(move(valuations))
    {

    }

    std::map<identity<law::property>, variable> excess_demand(
    const std::map<identity<law::property>,
                   std::tuple<quote, variable>> &quotes)
    const  override
    {
        std::map<identity<law::property>, variable> signals_;
        if(valuations.empty()) {
            return signals_;
        }
        variable position_        = 0;
        variable abs_sensitivity_ = 0;
        for(const auto &[k, v] : valuations) {
            
            auto subjective_  = double(v);
            auto quote_price_ = static_cast<double>(std::get<0>(quotes.find(k)->second));
            auto proposed_    = quote_price_ * std::get<1>(quotes.find(k)->second);
            auto signal_      = (subjective_ - proposed_);
            signals_.insert({k, signal_});
            position_ += signal_;
            abs_sensitivity_ += abs(signal_);
        }

        std::map<identity<law::property>, variable> excess_demand_;
        for(const auto &[k, v] : signals_) {
            auto quote_price_ = static_cast<double>(std::get<0>(quotes.find(k)->second));
            auto proposed_    = quote_price_ * std::get<1>(quotes.find(k)->second);

            double supply_ = 0;
            auto iterator_ = supply.find(k);
            if(supply.end() != iterator_) {
                supply_ = double(iterator_->second);
            }
            auto excess_ = (v * capital) / proposed_ - supply_;
            excess_demand_.insert({k, excess_});
        }
        return excess_demand_;
    }

    template<class archive_t>
    void serialize(archive_t &archive, const unsigned int version)
    {
        (void)version;
        archive &BOOST_SERIALIZATION_NVP(capital);
        archive &boost::serialization::make_nvp(
            "allocation",
            const_cast<std::remove_const<decltype(allocation)>::type &>(allocation));
        archive &boost::serialization::make_nvp(
            "supply",
            const_cast<std::remove_const<decltype(supply)>::type &>(supply));
        archive &boost::serialization::make_nvp(
            "valuations",
            const_cast<std::remove_const<decltype(valuations)>::type &>(valuations));
    }
};


struct test_constant_demand_trader
: public shareholder
{
    test_constant_demand_trader(
        const identity<shareholder> &i)
    : economics::finance::shareholder(i)
    {

    }

    simulation::time_point act( simulation::time_interval step
                              , std::seed_seq &seed
                              ) override
    {
        (void) seed;
        accounting::standard a(currencies::USD);

        for(auto [k, message_]: inbox) {
            switch(k) {
            case economics::markets::walras::quote_message::code
                :
                auto quote_ = std::dynamic_pointer_cast<
                    economics::markets::walras::quote_message>(message_);

                size_t assets = quote_->proposed.size();

                std::vector<double> allocation =
                    std::vector<double>(assets, 1.0 / assets);
                std::vector<double> valuation =
                    std::vector<double>(assets, 100.0);
                std::vector<double> signal;

                for(size_t a = 0; a < assets; ++a) {
                    signal.push_back(abs(4 /*distribution_(generator_)*/));
                }

                //auto response_ = std::make_shared<test_trader_order>(
                //    double(1)
                //    ,
                 //   allocation, signal, valuation, *quote_);
                //this->outbox.push_back(response_);
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
BOOST_AUTO_TEST_CASE(walras_market_quote)
{
    computation::environment e;
    simulation::model model_(e, simulation::parameter::parametrization(0, 0, 10));


    std::vector<std::tuple<std::shared_ptr<company>, share_class>>
        share_classes_;

    typedef std::tuple<identity<company>,  economics::finance::share_class> key_t;
    std::map<key_t, identity<law::property>> stocks_;

    law::property_map<economics::markets::quote> traded_assets_;
    size_t assets = 2;

    for(size_t a = 0; a < assets; ++a) {
        auto company_identifier_ =
            model_.template create_identifier<economics::company>();

        auto company_ = std::make_shared<economics::company>(
            company_identifier_, law::jurisdictions::US);

        auto main_issue_ = economics::finance::share_class();
        //company_->dividend_payouts = create_output<price>("dividend");
        company_->shares_outstanding[main_issue_] = 1'000'000;

        for(const auto &[share_, quantity] : company_->shares_outstanding) {
            (void) quantity;
            auto stock_ = std::make_shared<economics::finance::stock>(
                *company_, share_);
            traded_assets_.insert(
                {stock_, economics::markets::quote(economics::price(
                             1.00, economics::currencies::USD))});
            share_classes_.emplace_back(make_tuple(company_, share_));

            auto key_ = std::make_tuple<identity<economics::company>,
                                        economics::finance::share_class>(
                *company_, economics::finance::share_class(share_));
            stocks_.insert({key_, stock_->identifier});
        }
    }


    auto market_ =
        model_.template create<economics::markets::walras::price_setter>();

    market_->traded_properties = traded_assets_;


    auto participants_ =
        std::vector<std::shared_ptr<economics::finance::shareholder>>(
            {model_.template create<economics::finance::shareholder>()});


    for(auto &p : participants_) {
        market_->participants.insert(*p);

        for(const auto &[k, v] : stocks_) {
            p->stocks.insert(std::make_pair(k, v));
        }

        for(const auto &[k, v] : traded_assets_) {
            (void)v;
            (*p).economics::finance::shareholder::
                owner<economics::finance::stock>::properties.insert(
                    std::dynamic_pointer_cast<economics::finance::stock>(
                        k),
                    quantity(100'000'000));
        }

        std::cout << share_classes_.size() << std::endl;

        for(auto &share: share_classes_) {
            auto c = std::get<0>(share);

            std::map<finance::share_class, std::uint64_t> holdings_;
            holdings_.emplace(std::get<1>(share), 100'000'000);

            c->shareholders.emplace(*p, holdings_);
        }
    }

    model_.step({0, 1});

    model_.step({1, 2});
    model_.step({1, 3});
    BOOST_CHECK_EQUAL(participants_[0]->prices.size(), 2);

    std::cout << participants_[0]->prices << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()  // ESL
