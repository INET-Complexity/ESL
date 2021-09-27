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
#define BOOST_TEST_MODULE sedol

#include <boost/test/included/unit_test.hpp>

#include <esl/economics/finance/sedol.hpp>
using namespace esl::economics::finance;


BOOST_AUTO_TEST_SUITE(ESL)


BOOST_AUTO_TEST_CASE(sedol_test_checksum)
{

    {
        std::array<char, 6> code_ = {'7', '1', '0', '8', '8', '9'};

        sedol c(code_);
        BOOST_CHECK_EQUAL(c.checksum(), '9');
    }

    {
        sedol c("B0YBKJ");
        BOOST_CHECK_EQUAL(c.checksum(), '7');
    }

    {
        sedol c("4065663");
        BOOST_CHECK_EQUAL(c.checksum(), '3');
    }

    {
        sedol c("B0YBLH");
        BOOST_CHECK_EQUAL(c.checksum(), '2');
    }

    {
        sedol c("228276");
        BOOST_CHECK_EQUAL(c.checksum(), '5');
    }

    {
        sedol c("B0YBKL");
        BOOST_CHECK_EQUAL(c.checksum(), '9');
    }

    {
        sedol c("557910");
        BOOST_CHECK_EQUAL(c.checksum(), '7');
    }

    {
        sedol c("B0YBKR");
        BOOST_CHECK_EQUAL(c.checksum(), '5');
    }

    {
        sedol c("585284");
        BOOST_CHECK_EQUAL(c.checksum(), '2');
    }

    {
        sedol c("B0YBKT");
        BOOST_CHECK_EQUAL(c.checksum(), '7');
    }

}

BOOST_AUTO_TEST_SUITE_END()  // ESL
