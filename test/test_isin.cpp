/// \file   test_isin.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-29
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
#define BOOST_TEST_MODULE isin

#include <boost/test/included/unit_test.hpp>

#include <esl/economics/finance/isin.hpp>
#include <esl/geography/countries.hpp>
using namespace esl::economics::finance;
using namespace esl::geography;

BOOST_AUTO_TEST_SUITE(ESL)


    BOOST_AUTO_TEST_CASE(identity_equality_different_types)
    {
        {
            // us0378331005 apple inc
            std::array<char, 9> cusip_ = {'0', '3', '7', '8', '3',
                                     '3', '1', '0', '0'};
            isin i(countries::US, cusip_);
            BOOST_CHECK_EQUAL(i.checksum(), '5');
        }

        {
            // au0000xvgza3 victoria treasury
            std::array<char, 9> cusip_ = {'0', '0', '0', '0', 'X',
                                     'V', 'G', 'Z', 'A'};
            isin i(countries::AU, cusip_);
            BOOST_CHECK_EQUAL(i.checksum(), '3');
        }

        {
            // gb0002634946 bae systems
            std::array<char, 9> cusip_ = {'0', '0', '0', '2', '6',
                                     '3', '4', '9', '4'};
            isin i(countries::GB, cusip_);
            BOOST_CHECK_EQUAL(i.checksum(), '6');
        }
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL
