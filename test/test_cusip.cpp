/// \file   test_cusip.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2021-09-17
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
#define BOOST_TEST_MODULE cusip

#include <boost/test/included/unit_test.hpp>

#include <esl/economics/finance/cusip.hpp>
using namespace esl::economics::finance;


BOOST_AUTO_TEST_SUITE(ESL)


BOOST_AUTO_TEST_CASE(cusip_test_checksum)
{

    {
        // us0378331005 Apple Inc.
        // ^^           ISIN country
        //   ^^^^^^^^^  CUSIP part
        //   ^^^^^^     issuer
        //         ^^   code
        //           ^  CUSIP checksum
        //            ^ ISIN checksum

        std::array<char, 6> issuer_ = {'0', '3', '7', '8', '3','3'};
        std::array<char, 2> code_ = {'1', '0'};

        cusip c(issuer_, code_);
        BOOST_CHECK_EQUAL(c.checksum(), '0');
    }
    {   // this tests that "10" is the default issue number
        // Cisco Systems: 17275R102
        std::array<char, 6> issuer_ = {'1', '7', '2', '7', '5','R'};
        cusip c(issuer_);
        BOOST_CHECK_EQUAL(c.checksum(), '2');
    }
    {
        // Google Inc.: 38259P508
        std::array<char, 9> cusip_ = {'3', '8', '2', '5', '9','P', '5', '0', '8'};
        cusip c(cusip_);
        BOOST_CHECK_EQUAL(c.checksum(), '8');
    }
    {
        // Microsoft Corporation: 594918104
        std::array<char, 9> cusip_ = {'5', '9', '4', '9', '1','8', '1', '0', '4'};
        cusip c(cusip_);
        BOOST_CHECK_EQUAL(c.checksum(), '4');
    }
    {   // this tests constructor from constexpr string
        // Oracle Corporation: 68389X105
        //std::array<char, 9> cusip_ = {'6', '8', '3', '8', '9','X', '1', '0', '5'};
        cusip c("68389X105");
        BOOST_CHECK_EQUAL(c.checksum(), '5');
    }
    {
        // 3½% Treasury Gilt 2068: 00BBJNQY2
        std::array<char, 8> cusip_ = {'0', '0', 'B', 'B', 'J','N', 'Q', 'Y'};
        cusip c(cusip_);
        BOOST_CHECK_EQUAL(c.checksum(), '2');
    }
}

BOOST_AUTO_TEST_SUITE_END()  // ESL
