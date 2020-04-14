/// \file   test_automatically_differentiable_variable.cpp
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
#define BOOST_TEST_MODULE variable

#include <boost/test/included/unit_test.hpp>

#include <esl/computation/environment.hpp>
using esl::computation::environment;
#include <esl/simulation/model.hpp>
using esl::simulation::model;
using esl::simulation::parameter::parametrization;
#include <esl/economics/finance/bond.hpp>

using esl::identity;
using namespace esl::law;
#include <esl/law/government.hpp>
using esl::law::government;
#include <esl/law/legal_person.hpp>
using esl::law::legal_person;
using esl::law::property;
using esl::law::property_map;


#include <esl/law/jurisdictions.hpp>
using esl::economics::currencies::USD;


#include <esl/simulation/time.hpp>
using namespace esl::simulation;

#include <esl/economics/price.hpp>
using esl::economics::price;
#include <esl/economics/markets/quote.hpp>
using esl::economics::quote;
#include <esl/economics/finance/company.hpp>
using esl::economics::company;
using esl::economics::cash;
using esl::economics::finance::bond;
using esl::economics::finance::stock;
using esl::economics::finance::bond;
using esl::economics::finance::shareholder;

#include <esl/mathematics/variable.hpp>

#include <esl/economics/rate.hpp>
using esl::economics::rate;


#include <esl/economics/markets/walras/tatonnement.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
using esl::economics::markets::walras::price_setter;


#include <esl/economics/interest_rate.hpp>
using esl::economics::nominal_interest_rate;


BOOST_AUTO_TEST_SUITE(ESL)

BOOST_AUTO_TEST_CASE(variable_constructor)
    {
        std::vector<esl::variable> v = {1.0, 3.0, 5.0, 7.0};
        for(auto x : v){
            //x.();
            //auto f = -(x * x * x);
            //f.grad();
            //BOOST_CHECK_CLOSE(f.val(), - (x.val() * x.val() * x.val()), 0.000'000'1);
            //BOOST_CHECK_CLOSE(x.adj(), -3. * (x.val()  * x.val() ) + 1.0, 0.000'000'1);
        }




        environment e;
        model model_(e, parametrization(0, 0, 3));

        std::vector<std::tuple<std::shared_ptr<company>, share_class>>             shares_;
        std::map<std::tuple<identity<company>, share_class>, identity<property>>    stocks_;
        property_map<quote>                                                         traded_assets_;
        property_map<size_t>                                                        shares_outstanding_;

        auto government_ = model_.template create<legal_person>(government("Department of the Treasury"), jurisdictions::US);
        auto fixed_coupon_ = std::make_shared<bond::fixed_coupon>();
        fixed_coupon_->rate = nominal_interest_rate(rate<int64_t>(0.01), time_duration(252));
        time_point maturity_ = 10000;
        identity<bond> bond_id_ = (*government_).template create<bond>();
        auto bond_ = std::make_shared<bond>(bond_id_, maturity_, fixed_coupon_, *government_);
        std::cout << bond_->name() << std::endl;
        std::map<identity<property>, quote> initial_quotes_;
        initial_quotes_.emplace( bond_->identifier,quote(price(1.00, USD)));
        tatonnement::excess_demand_model edmodel_(initial_quotes_);
        std::map<identity<property>, price> valuations_;
        valuations_.emplace(bond_->identifier, price(1.50, USD));


/*
        auto dm = std::make_shared<dividend_discount_ddsf>
            (esl::identity<esl::agent>()
                , esl::identity<esl::agent>()
                , 0,0, valuations_);

        edmodel_.excess_demand_functions_.push_back(dm);
*/
        edmodel_.do_compute();
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL