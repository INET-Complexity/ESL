/// \file   test_algorithms.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-30
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
#define BOOST_TEST_MODULE algorithms

#include <boost/test/included/unit_test.hpp>

#include <esl/algorithms.hpp>

#include <list>
#include <set>
#include <vector>


BOOST_AUTO_TEST_SUITE(ESL)

    BOOST_AUTO_TEST_CASE(to_array_vector)
    {
        std::vector<int> vector_empty_ = {};
        auto vector_a_empty_           = esl::to_array<0, 0, char>(vector_empty_);
        BOOST_CHECK(vector_a_empty_.size() == 0);

        std::vector<int> vector_ = {1, 2, 3, 4, 5};
        auto vector_a_           = esl::to_array<1, 3, char>(vector_);
        BOOST_CHECK(vector_a_.size() == 2);
        std::array<char, 2> expected_ = {2, 3};
        BOOST_CHECK(vector_a_ == expected_);
    }

    BOOST_AUTO_TEST_CASE(to_array_list)
    {
        std::list<int> list_ = {1, 2, 3, 4, 5};
        auto list_a_         = esl::to_array<1, 5, char>(list_);
        BOOST_CHECK(list_a_.size() == 4);
        std::array<char, 4> expected_ = {2, 3, 4, 5};
        BOOST_CHECK(list_a_ == expected_);
    }

    BOOST_AUTO_TEST_CASE(to_array_set)
    {
        std::set<int> set_ = {5, 2, 3, 4, 1};
        auto set_a_        = esl::to_array<1, 4, char>(set_);
        BOOST_CHECK(set_a_.size() == 3);

        std::array<char, 3> expected_ = {2, 3, 4};
        BOOST_CHECK(set_a_ == expected_);
    }

    BOOST_AUTO_TEST_CASE(array_concatenate)
    {
        std::array<char, 0> a = {};
        std::array<char, 1> b = {1};
        std::array<char, 2> c = {2, 3};

        std::array<char, 3> expected_abc_ = {1, 2, 3};

        auto aa = esl::array_concatenate(a,a);
        BOOST_CHECK(aa == a);

        auto ab = esl::array_concatenate(a,b);
        BOOST_CHECK(ab == b);

        auto abc = esl::array_concatenate(ab,c);
        BOOST_CHECK(abc == expected_abc_);
    }

    BOOST_AUTO_TEST_CASE(array_slice)
    {
        std::array<char, 3> s = {1, 2, 3};
        std::array<char, 0> e = {};
        auto r0 = esl::array_slice<0,0>(s);
        BOOST_CHECK(r0 == e);


        auto r1 = esl::array_slice<0,3>(s);
        BOOST_CHECK(r1 == s);

        std::array<char, 1> m = {2};
        auto r2 = esl::array_slice<1,2>(s);
        BOOST_CHECK(r2 == m);
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL
