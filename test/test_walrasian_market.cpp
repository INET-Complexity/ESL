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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE walras

#include <boost/test/included/unit_test.hpp>

#include <esl/economics/finance/company.hpp>


#include <esl/computation/environment.hpp>
#include <esl/economics/finance/shareholder.hpp>
#include <esl/economics/finance/stock.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
#include <esl/law/jurisdictions.hpp>
#include <esl/simulation/model.hpp>

#include <esl/economics/markets/walras/quote_message.hpp>
#include <esl/economics/markets/walras/order_message.hpp>

struct test_share_issuing_company : public esl::economics::company
{
public:
    test_share_issuing_company(const esl::identity<esl::economics::company> &i)
    : esl::economics::company(i, esl::law::jurisdictions::US)
    {
        auto main_issue_                = esl::economics::finance::share();
        shares_outstanding[main_issue_] = 1'000'000;
        // auto dividend_payouts =
        // create_output<esl::economics::price>("dividend");
    }
};


struct test_trader_order
: public esl::economics::markets::walras::differentiable_order_message
{
    double capital;

    const std::vector<double> allocation;

    std::map<esl::identity<esl::law::property>, esl::quantity> supply;

    std::map<esl::identity<esl::law::property>, esl::economics::price>
        valuations;

    using esl::economics::markets::walras::differentiable_order_message::
        differentiable_order_message;

    test_trader_order(
        double capital = 0.0, std::vector<double> allocation = {},
        std::map<esl::identity<esl::law::property>, esl::quantity> supply = {},
        std::map<esl::identity<esl::law::property>, esl::economics::price>
            valuations = {}  // vector<double> valuation = {}
        ,
        const esl::economics::markets::walras::quote_message &q =
            esl::economics::markets::walras::quote_message())

    : esl::economics::markets::walras::differentiable_order_message(q.sender, q.recipient)
    , capital(capital)
    , allocation(move(allocation))
    , supply(move(supply))
    , valuations(move(valuations))
    {

    }

    std::map<esl::identity<esl::law::property>, esl::variable> excess_demand_m(
    const std::map<esl::identity<esl::law::property>,
                   std::tuple<esl::economics::quote, esl::variable>> &quotes)
    const  override
    {
        std::map<esl::identity<esl::law::property>, esl::variable> signals_;
        if(valuations.empty()) {
            return signals_;
        }
        /// We make the simplifying assumption that the assets of a long
        /// position can count as collateral towards a short position's margin
        /// account.
        esl::variable position_        = 0;
        esl::variable abs_sensitivity_ = 0;
        for(const auto &[k, v] : valuations) {
            
            auto subjective_  = double(v);
            auto quote_price_ = static_cast<double>(std::get<0>(quotes.find(k)->second));
            auto proposed_    = quote_price_ * std::get<1>(quotes.find(k)->second);
            auto signal_      = (subjective_ - proposed_);
            signals_.insert({k, signal_});
            position_ += signal_;
            abs_sensitivity_ += abs(signal_);
        }
        /// The mean position represents the working capital employed, examples:
        ///    -1.0 the short position is equal to working capital, securities =
        ///    -1.0, cash = 2.0, margin = 1.0
        ///     0.0 no trading, or long == short
        ///     1.0 the long position uses all the working capital, securities
        ///     =  1.0, cash = 0.0, margin = 0.0
        //    auto risk_limit_ = 1.; // make-shift risk limit
        // variable scale_ = risk_limit_ / (abs_sensitivity_ / q);
        std::map<esl::identity<esl::law::property>, esl::variable> excess_demand_;
        for(const auto &[k, v] : signals_) {
            auto quote_price_ = static_cast<double>(std::get<0>(quotes.find(k)->second));
            auto proposed_    = quote_price_ * std::get<1>(quotes.find(k)->second);

            double supply_ = 0;
            auto iterator_ = supply.find(k);
            if(supply.end() != iterator_) {
                supply_ = double(iterator_->second);
            }  // we allow not setting the supply, in case there are no holdings
            auto excess_ = (v * capital) / proposed_ - supply_;
            excess_demand_.insert({k, excess_});
        }
        return excess_demand_;
    }

    template<class archive_t>
    void serialize(archive_t &archive, const unsigned int version)
    {
        (void)version;

        // archive &
        // BOOST_SERIALIZATION_BASE_OBJECT_NVP(walras::differentiable_order_message);
        // // base is abstract

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
: public esl::economics::finance::shareholder
{
    test_constant_demand_trader(
        const esl::identity<esl::economics::finance::shareholder> &i)
    : esl::economics::finance::shareholder(i)
    {}

    esl::simulation::time_point act(esl::simulation::time_interval step,
                                    std::seed_seq &seed) override
    {
        (void) seed;
        esl::economics::accounting::standard a(esl::economics::currencies::USD);
        auto wealth1_ =
            esl::law::owner<esl::economics::cash>::properties.value(a);
        auto wealth2_ =
            esl::law::owner<esl::economics::finance::stock>::properties.value(
                a);

        auto working_capital_ = wealth1_ + wealth2_;

        (void) working_capital_;

        for(auto [k, message_] : inbox) {
            switch(k) {
            case esl::economics::markets::walras::quote_message::code
                :
                auto quote_ = std::dynamic_pointer_cast<
                    esl::economics::markets::walras::quote_message>(message_);

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
    esl::computation::environment e;
    esl::simulation::model model_(e, esl::simulation::parameter::parametrization(0, 0, 1));


    std::vector<std::tuple<std::shared_ptr<esl::economics::company>,
                           esl::economics::finance::share>>
        shares_;

    std::map<std::tuple<esl::identity<esl::economics::company>,
                        esl::economics::finance::share>,
             esl::identity<esl::law::property>>
        stocks_;

    esl::law::property_map<esl::economics::quote> traded_assets_;
    size_t assets = 2;

    for(size_t a = 0; a < assets; ++a) {
        auto company_identifier_ =
            model_.template create_identifier<esl::economics::company>();

        auto company_ = std::make_shared<esl::economics::company>(
            company_identifier_, esl::law::jurisdictions::US);

        auto main_issue_ = esl::economics::finance::share();
        //company_->dividend_payouts = create_output<price>("dividend");
        company_->shares_outstanding[main_issue_] = 1'000'000;

        for(const auto &[share_, quantity] : company_->shares_outstanding) {
            (void) quantity;
            auto stock_ = std::make_shared<esl::economics::finance::stock>(
                *company_, share_);
            traded_assets_.insert(
                {stock_, esl::economics::quote(esl::economics::price(
                             100, esl::economics::currencies::USD))});
            shares_.emplace_back(make_tuple(company_, share_));

            auto key_ = std::make_tuple<esl::identity<esl::economics::company>,
                                        esl::economics::finance::share>(
                *company_, esl::economics::finance::share(share_));
            stocks_.insert({key_, stock_->identifier});
        }
    }


    auto market_ =
        model_.template create<esl::economics::markets::walras::price_setter>();

    market_->traded_properties = traded_assets_;


    auto participants_ =
        std::vector<std::shared_ptr<esl::economics::finance::shareholder>>(
            {model_.template create<esl::economics::finance::shareholder>()});


    for(auto &p : participants_) {
        market_->participants.insert(*p);

        for(const auto &[k, v] : stocks_) {
            p->stocks.insert(std::make_pair(k, v));
        }

        for(const auto &[k, v] : traded_assets_) {
            (void)v;
            (*p).esl::economics::finance::shareholder::
                owner<esl::economics::finance::stock>::properties.insert(
                    std::dynamic_pointer_cast<esl::economics::finance::stock>(
                        k),
                    esl::quantity(100'000'000));
        }

        std::cout << shares_.size() << std::endl;

        for(auto &share : shares_) {
            std::get<0>(share)->shareholders[*p] = {
                {std::get<1>(share), 100'000'000}};
        }
    }

    model_.step({0, 1});
    BOOST_CHECK_EQUAL(participants_[0]->prices.size(), 2);
}

BOOST_AUTO_TEST_SUITE_END()  // ESL
