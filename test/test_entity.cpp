/// \file   test_entity.cpp
///
/// \brief  This tests the semantic version string function.
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
#define BOOST_TEST_MODULE version

#include <boost/test/included/unit_test.hpp>

#include <esl/simulation/entity.hpp>
#include <utility>

BOOST_AUTO_TEST_SUITE(ESL)

struct dummy_base : esl::entity<dummy_base>
{
    explicit dummy_base(
        esl::identity<dummy_base> i = esl::identity<dummy_base>())
    : esl::entity<dummy_base>(std::move(i))
    {}

    virtual ~dummy_base() = default;
};

struct dummy_derived_direct
: dummy_base
, esl::identifiable_as<dummy_derived_direct>
{
    explicit dummy_derived_direct(esl::identity<dummy_derived_direct> i)
    : dummy_base(i)
    {}
};

BOOST_AUTO_TEST_CASE(entity_implicit_conversion)
{
    esl::identity<dummy_base> i           = {1, 2, 3};
    esl::identity<dummy_derived_direct> j = {1, 2, 3};

    dummy_derived_direct ddd {j};
    BOOST_CHECK_EQUAL(i, static_cast<esl::identity<dummy_base>>(ddd));
}


BOOST_AUTO_TEST_CASE(entity_create_child)
{
    esl::identity<dummy_base> i = {1, 2};
    dummy_base db(i);
    auto ddd0 = db.template create<dummy_derived_direct>();
    auto ddd1 = db.template create<dummy_derived_direct>();


    auto c0 = static_cast<esl::identity<dummy_base>>(ddd0);
    auto c1 = static_cast<esl::identity<dummy_base>>(ddd1);

    BOOST_CHECK_LT(c0, c1);
}

BOOST_AUTO_TEST_SUITE_END()  // ESL
