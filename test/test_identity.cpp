/// \file   test_identity.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-08-15
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
#define BOOST_TEST_MODULE identity

#include <boost/test/included/unit_test.hpp>


#include <esl/simulation/identifiable_as.hpp>
#include <esl/simulation/identity.hpp>

#include <string>
using std::string;
#include <sstream>


BOOST_AUTO_TEST_SUITE(ESL)

struct dummy_base : esl::identifiable_as<dummy_base>
{
    esl::identity<dummy_base> identifier;

    dummy_base(esl::identity<dummy_base> i = esl::identity<dummy_base>())
    : identifier(std::move(i))
    {}

    virtual ~dummy_base() = default;
};

struct dummy_derived_direct
: dummy_base
, esl::identifiable_as<dummy_derived_direct>
{
    dummy_derived_direct(const esl::identity<dummy_derived_direct> &i)
    : dummy_base(i)
    {}
};



void identity_initializer_list_initialize()
{
    esl::identity<dummy_base> i = {1, 2, 3};
}

BOOST_AUTO_TEST_CASE(identity_initializer_list)
{
    BOOST_CHECK_NO_THROW(identity_initializer_list_initialize());
}

BOOST_AUTO_TEST_CASE(identifiable_initialization)
{
    // this tests the basic constructor and is not the main test
    esl::identity<dummy_base> i = {1, 2, 3};
    dummy_base db               = {i};
    BOOST_CHECK_EQUAL(db.identifier, i);

    // here we test automatic identifier conversion during construction
    esl::identity<dummy_derived_direct> j = {4, 5, 6};
    dummy_derived_direct ddd              = {j};
    BOOST_CHECK_EQUAL(ddd.identifier, j);
}

BOOST_AUTO_TEST_CASE(identity_ostream_operator)
{
    esl::identity<dummy_base> i = {1, 0, 23, 456, 7};

    string reference2_ = "\"01-00-23-456-07\"";
    BOOST_CHECK_EQUAL(i.representation(2), reference2_);

    string reference0_ = "\"1-0-23-456-7\"";
    BOOST_CHECK_EQUAL(i.representation(0), reference0_);
}

BOOST_AUTO_TEST_CASE(identity_equality_different_instances)
{
    esl::identity<dummy_base> i = {1, 2, 3};
    esl::identity<dummy_base> j = {1, 2, 3};
    BOOST_CHECK_EQUAL(i, j);
    BOOST_CHECK_GE(i, i);
    BOOST_CHECK_GE(i, j);
    BOOST_CHECK_LE(i, i);
    BOOST_CHECK_LE(i, j);
    BOOST_CHECK(!(i != i));
    BOOST_CHECK(!(i != j));
}

BOOST_AUTO_TEST_CASE(identity_inequalities)
{
    esl::identity<dummy_base> i = {1, 2, 3};
    esl::identity<dummy_base> k = {1, 2, 4};
    BOOST_CHECK_LT(i, k);
    BOOST_CHECK(!(i == k));
    BOOST_CHECK_GT(k, i);

    // test i < l, k < l,
    esl::identity<dummy_base> l = {3, 2, 1};
    BOOST_CHECK_LT(i, l);
    BOOST_CHECK(!(i == l));
    BOOST_CHECK_GT(l, i);

    BOOST_CHECK_LT(k, l);
    BOOST_CHECK(!(k == l));
    BOOST_CHECK_GT(l, k);
}


BOOST_AUTO_TEST_CASE(identity_length_inequalities)
{
    esl::identity<dummy_base> i = {1, 2, 3};
    esl::identity<dummy_base> m = {1, 2};
    BOOST_CHECK_LT(m, i);
    BOOST_CHECK(!(i == m));
    BOOST_CHECK_GT(i, m);

    esl::identity<dummy_base> n = {1, 2, 3, 0};
    BOOST_CHECK_LT(i, n);
    BOOST_CHECK(!(i == n));
    BOOST_CHECK_GT(n, i);
}

BOOST_AUTO_TEST_CASE(identity_equality_different_types)
{
    esl::identity<dummy_base> i           = {1, 2, 3};
    esl::identity<dummy_derived_direct> j = {1, 2, 3};
    BOOST_CHECK_EQUAL(i, j);
    BOOST_CHECK(!(i < j));
    BOOST_CHECK(!(i > j));
}


BOOST_AUTO_TEST_SUITE_END()  // ESL
