/// \file   test_type_marker.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-17
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

#include <esl/computation/type_code.hpp>
#include <esl/computation/type_marker.hpp>


BOOST_AUTO_TEST_SUITE(ESL)

    template<typename unique_type_, std::uint64_t type_code_>
    struct force_unique
    : public esl::type_code<type_code_>
    , public esl::type_marker<unique_type_>
    {

        friend esl::type_marker<unique_type_> marker(esl::type_code<type_code_>)
        {
            return esl::type_marker<unique_type_>();
        }
    };

    struct u0
    : public force_unique<u0, 0>
    {

    };

    struct u123
    : public force_unique<u123, 123>
    {

    };

    //struct u123_duplicate
    //: public force_unique<u123_duplicate, 123>    // definition should result
    //{                                             // in compile error
    //
    //};

    struct u123_wrong_mapping
    : public force_unique<u123, 456>
    {

    };


    BOOST_AUTO_TEST_CASE(type_marker_code)
    {
        // we must capture the value here first, as the comparison macro below
        // can not cope with the static const
        auto x0 = u0::code;
        BOOST_CHECK_EQUAL(x0, 0);
        u0 u0_;
        (void)u0_;

        constexpr auto x123 = u123::code;
        BOOST_CHECK_EQUAL( x123, 123);
        u123 u123_;
        (void)u123_;

        //u123_duplicate u123d_; // should not compile when instantiated
        //(void) u123d_;
        u123_wrong_mapping u123wm_; // should still compile
        (void)u123wm_;

    }

BOOST_AUTO_TEST_SUITE_END()  // ESL
