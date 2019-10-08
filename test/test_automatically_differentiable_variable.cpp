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

#include <esl/mathematics/variable.hpp>


BOOST_AUTO_TEST_SUITE(ESL)

BOOST_AUTO_TEST_CASE(variable_constructor)
    {
        std::vector<esl::variable> v = {1.0, 3.0, 5.0, 7.0};
        for(auto x : v){
            x.grad();
            auto f = -(x * x * x);
            f.grad();
            BOOST_CHECK_CLOSE(f.val(), - (x.val() * x.val() * x.val()), 0.000'000'1);
            BOOST_CHECK_CLOSE(x.adj(), -3. * (x.val()  * x.val() ) + 1.0, 0.000'000'1);
        }
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL