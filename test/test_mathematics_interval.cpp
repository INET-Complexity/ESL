/// \file   test_mathematics_interval.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-09
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
#define BOOST_TEST_MODULE interval

#include <boost/test/included/unit_test.hpp>


#include <esl/mathematics/interval.hpp>
using esl::mathematics::interval;

BOOST_AUTO_TEST_SUITE(ESL)

BOOST_AUTO_TEST_CASE(interval_constructor)
{
    interval<int, false, false> i {-1, 3};

    BOOST_CHECK(i.lower == -1);
    BOOST_CHECK(i.upper == 3);
}

BOOST_AUTO_TEST_CASE(integer_interval)
{
    //     [l, u]
    {
        interval<int, false, false> i_lo_ro_n1 {2, 1};
        BOOST_CHECK(i_lo_ro_n1.empty());
        BOOST_CHECK(!i_lo_ro_n1.singleton());
        BOOST_CHECK(i_lo_ro_n1.degenerate());
    }

    {
        interval<int, false, false> i_lo_ro_0 {1, 1};
        BOOST_CHECK(!i_lo_ro_0.empty());
        BOOST_CHECK(i_lo_ro_0.singleton());
        BOOST_CHECK(i_lo_ro_0.degenerate());
    }

    {
        interval<int, false, false> i_lo_ro_0 {1, 2};
        BOOST_CHECK(!i_lo_ro_0.empty());
        BOOST_CHECK(!i_lo_ro_0.singleton());
        BOOST_CHECK(!i_lo_ro_0.degenerate());
    }
    //     (l, u]
    {
        interval<int, true, false> i_lc_ro_0 {1, 1};
        BOOST_CHECK(i_lc_ro_0.empty());
        BOOST_CHECK(!i_lc_ro_0.singleton());
        BOOST_CHECK(i_lc_ro_0.degenerate());
    }

    {
        interval<int, true, false> i_lc_ro_1 {1, 2};
        BOOST_CHECK(!i_lc_ro_1.empty());
        BOOST_CHECK(i_lc_ro_1.singleton());
        BOOST_CHECK(i_lc_ro_1.degenerate());
    }
    //     [l, u)
    {
        interval<int, false, true> i_lo_rc_0 {1, 1};
        BOOST_CHECK(i_lo_rc_0.empty());
        BOOST_CHECK(!i_lo_rc_0.singleton());
        BOOST_CHECK(i_lo_rc_0.degenerate());
    }

    {
        interval<int, false, true> i_lo_rc_1 {1, 2};
        BOOST_CHECK(!i_lo_rc_1.empty());
        BOOST_CHECK(i_lo_rc_1.singleton());
        BOOST_CHECK(i_lo_rc_1.degenerate());
    }
    //     (l, u)
    {
        interval<int, true, true> i_lc_rc_0 {1, 1};
        BOOST_CHECK(i_lc_rc_0.empty());
        BOOST_CHECK(!i_lc_rc_0.singleton());
        BOOST_CHECK(i_lc_rc_0.degenerate());
    }

    {
        interval<int, true, true> i_lc_rc_1 {1, 2};
        BOOST_CHECK(i_lc_rc_1.empty());
        BOOST_CHECK(!i_lc_rc_1.singleton());
        BOOST_CHECK(i_lc_rc_1.degenerate());
    }

    {
        interval<int, true, true> i_lc_rc_2 {1, 3};
        BOOST_CHECK(!i_lc_rc_2.empty());
        BOOST_CHECK(i_lc_rc_2.singleton());
        BOOST_CHECK(i_lc_rc_2.degenerate());
    }
}  // BOOST_AUTO_TEST_CASE integer_interval

BOOST_AUTO_TEST_SUITE_END()  // ESL
