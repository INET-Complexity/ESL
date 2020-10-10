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
using esl::economics::markets::quote;
#include <esl/economics/finance/company.hpp>
using esl::economics::company;
using esl::economics::cash;
using esl::economics::finance::bond;
using esl::economics::finance::stock;
using esl::economics::finance::bond;

using esl::economics::finance::share_class;
using esl::economics::finance::shareholder;

#include <esl/mathematics/variable.hpp>

#include <esl/economics/rate.hpp>
using esl::economics::rate;


#include <esl/economics/markets/walras/tatonnement.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
using esl::economics::markets::walras::price_setter;
using namespace esl::economics::markets::tatonnement;

using esl::economics::nominal_interest_rate;


BOOST_AUTO_TEST_SUITE(ESL)

BOOST_AUTO_TEST_CASE(variable_constructor)
    {
        /*
        adept::Stack stack_;
        std::vector<double> v = {1.0, 3.0, 5.0, 7.0};
        for(auto x1 : v){

            esl::variable x = x1;
            stack_.new_recording();

            adept::adouble f = -(x * x * x);

            stack_.independent(&x, 1);
            stack_.dependent(&x, 1);
            double j[1] = {0.};
            stack_.jacobian(j);


            f.set_gradient(1.0);
            stack_.compute_adjoint();
            double dj[1] = {0.};
            adept::get_gradients(&x, 1, dj);

            BOOST_CHECK_CLOSE(adept::value(f), - (adept::value(x) * adept::value(x) * adept::value(x)), 0.000'000'1);

            BOOST_CHECK_CLOSE(dj[0], -3. * (adept::value(x)  * adept::value(x) ) , 0.000'000'1);
        }
        */

    }

BOOST_AUTO_TEST_SUITE_END()  // ESL