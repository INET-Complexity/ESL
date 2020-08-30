/// \file   test_quantity.cpp
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
#define BOOST_TEST_MODULE quantity

#include <boost/test/included/unit_test.hpp>

#include <esl/quantity.hpp>


BOOST_AUTO_TEST_SUITE(ESL)

BOOST_AUTO_TEST_CASE(quantity_constructor)
{
    {
        esl::quantity q_default_;

        BOOST_CHECK_EQUAL(q_default_.amount, 0);
        BOOST_CHECK_EQUAL(q_default_.basis, 1);
    }

    BOOST_CHECK_THROW(esl::quantity(13, 0), std::logic_error);

    {
        esl::quantity one_(1);

        BOOST_CHECK_EQUAL(one_.amount, 1);
        BOOST_CHECK_EQUAL(one_.basis, 1);
    }

    {
        esl::quantity threefifty_(350, 100);

        BOOST_CHECK_EQUAL(threefifty_.amount, 350);
        BOOST_CHECK_EQUAL(threefifty_.basis, 100);


        esl::quantity again_(threefifty_);

        BOOST_CHECK_EQUAL(again_.amount, 350);
        BOOST_CHECK_EQUAL(again_.basis, 100);
    }
}

BOOST_AUTO_TEST_CASE(quantity_arithmetic_guards)
{
    // test exception on adding incompatible quantities
    esl::quantity threefifty_(350, 100);
#if EXCEPTION_ON_BASIS_CHANGE
    BOOST_CHECK_THROW((void)(threefifty_ + esl::quantity(1, 42)), std::logic_error);
#else
    BOOST_CHECK_EQUAL(threefifty_ + esl::quantity(1, 42),
                      esl::quantity(148, 42));
#endif


    // test exception on subtracting with negative result
    BOOST_CHECK_THROW((void)(threefifty_ - esl::quantity(351, 100)), std::logic_error);
    BOOST_CHECK_THROW((void)(threefifty_ - esl::quantity(176, 50)), std::logic_error);
    BOOST_CHECK_THROW((void)(threefifty_ - esl::quantity(701, 200)), std::logic_error);

    // test conversion on subtraction
#if EXCEPTION_ON_BASIS_CHANGE
    BOOST_CHECK_THROW((void)(threefifty_ - esl::quantity(1, 42)), std::logic_error);
#else
    BOOST_CHECK_EQUAL(threefifty_ - esl::quantity(1, 42),
                      esl::quantity(146, 42));

    BOOST_CHECK_EQUAL(threefifty_ - esl::quantity(3, 1),
                      esl::quantity(50, 100));
#endif


#if EXCEPTION_ON_BASIS_CHANGE
    {
        esl::quantity again_(threefifty_);
        BOOST_CHECK_THROW(again_ += esl::quantity(1, 42), std::logic_error);
    }
    {
        esl::quantity again_(threefifty_);
        BOOST_CHECK_THROW(again_ -= esl::quantity(1, 42), std::logic_error);
    }
#endif
}

BOOST_AUTO_TEST_CASE(quantity_arithmetic)
{
    esl::quantity threefifty_(350, 100);

    // .divide integer
    {
        auto constexpr_d_ = threefifty_.divide<4>();
        BOOST_CHECK_EQUAL(constexpr_d_[0], esl::quantity(88, 100));
        BOOST_CHECK_EQUAL(constexpr_d_[1], esl::quantity(88, 100));
        BOOST_CHECK_EQUAL(constexpr_d_[2], esl::quantity(87, 100));
        BOOST_CHECK_EQUAL(constexpr_d_[3], esl::quantity(87, 100));
    }

    // / integer
    {
        auto dynamic_d_ = threefifty_ / 3;
        auto q_117_     = esl::quantity(117, 100);
        auto q_116_     = esl::quantity(116, 100);
        BOOST_CHECK_EQUAL(dynamic_d_[0], q_117_);
        BOOST_CHECK_EQUAL(dynamic_d_[1], q_117_);
        BOOST_CHECK_EQUAL(dynamic_d_[2], q_116_);
    }

    // +
    BOOST_CHECK_EQUAL(threefifty_ + esl::quantity(123, 100),
                      esl::quantity(473, 100));
    BOOST_CHECK_EQUAL(threefifty_ + esl::quantity(0, 100),
                      esl::quantity(350, 100));
    // -
    BOOST_CHECK_EQUAL(threefifty_ - esl::quantity(123, 100),
                      esl::quantity(227, 100));
    // * integer
    BOOST_CHECK_EQUAL(threefifty_ * 3, esl::quantity(1050, 100));
    // * quantity
    BOOST_CHECK_EQUAL(threefifty_ * esl::quantity(50, 100),
                      esl::quantity(175, 100));
    BOOST_CHECK_EQUAL(threefifty_ * esl::quantity(200, 100),
                      esl::quantity(700, 100));
    BOOST_CHECK_EQUAL(threefifty_ * esl::quantity(0, 100),
                      esl::quantity(0, 100));
    BOOST_CHECK_EQUAL(threefifty_ * esl::quantity(100, 100),
                      esl::quantity(350, 100));
    BOOST_CHECK_EQUAL(threefifty_ * esl::quantity(10, 100),
                      esl::quantity(35, 100));
}


BOOST_AUTO_TEST_CASE(quantity_logic)
{
    esl::quantity threefifty_(350, 100);

    BOOST_CHECK_EQUAL(threefifty_, esl::quantity(350, 100));
    BOOST_CHECK_NE(threefifty_, esl::quantity(123, 100));

    BOOST_CHECK_LE(threefifty_, esl::quantity(456, 100));
    BOOST_CHECK_GE(threefifty_, esl::quantity(123, 100));

    BOOST_CHECK_EQUAL(threefifty_, 3.5);

    BOOST_CHECK_EQUAL(esl::quantity(142'857'142'857, 100'000'000'000),
                      10.0 / 7);
}


BOOST_AUTO_TEST_SUITE_END()  // ESL
