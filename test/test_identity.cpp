/// \file   test_identity.cpp
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

#include <esl/simulation/identity.hpp>

#include <string>
#include <sstream>


BOOST_AUTO_TEST_SUITE(ESL)

struct dummy_base
{

};

struct dummy_derived_direct
: dummy_base
{

};

struct dummy_derived_virtual
: virtual dummy_base
{

};

struct dummy_orphan
{

};

void identity_initializer_list_initialize()
{
    esl::identity<dummy_base> i = {1, 2, 3};
}


BOOST_AUTO_TEST_CASE(identity_initializer_list)
{
    BOOST_CHECK_NO_THROW(identity_initializer_list_initialize());
}

BOOST_AUTO_TEST_CASE(identity_ostream_operator)
{

    esl::identity<dummy_base> i = {1, 0, 23, 456, 7};

    string reference_ = "01-00-23-456-07";

    BOOST_CHECK_EQUAL(i.representation(2), reference_);

}

BOOST_AUTO_TEST_SUITE_END()  // ESL
