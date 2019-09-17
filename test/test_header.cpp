/// \file   test_header.cpp
///
/// \brief  This tests the semantic version string function.
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-01
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
#define BOOST_TEST_MODULE version

#include <boost/test/included/unit_test.hpp>

#include <esl/interaction/header.hpp>


#include <string>
using std::string;
#include <sstream>



BOOST_AUTO_TEST_SUITE(ESL)
    BOOST_AUTO_TEST_CASE(header_static_assertions)
    {
        // these test cases trigger the static assertions at compile time
        constexpr auto code0_ = esl::interaction::library_message_code<0>();
        BOOST_CHECK_EQUAL(code0_, (0x1ul << (std::numeric_limits<esl::interaction::message_code>::digits - 1ul)) | 0ul);

        constexpr auto code1_ = esl::interaction::library_message_code<1>();
        BOOST_CHECK_EQUAL(code1_, (0x1ul << (std::numeric_limits<esl::interaction::message_code>::digits - 1ul)) | 1ul);
        constexpr auto decode1_ = esl::interaction::library_message_offset<code1_>();
        BOOST_CHECK_EQUAL(decode1_,  1ul);
    }

    BOOST_AUTO_TEST_CASE(header_default_constructor)
    {
        esl::interaction::header h;
    }


    BOOST_AUTO_TEST_CASE(header_constructor)
    {
        esl::interaction::header h = {3, {1,2}, {3,2}, 0, 1};
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL
