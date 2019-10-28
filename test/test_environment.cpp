/// \file   test_environment.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-22
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
#define BOOST_TEST_MODULE environment

#include <boost/test/included/unit_test.hpp>

#include <esl/computation/environment.hpp>
#include <esl/simulation/model.hpp>
#include <esl/agent.hpp>


struct test_agent
: public esl::agent
{
    using esl::agent::agent;

    esl::simulation::time_point act(esl::simulation::time_interval step,
                                    std::seed_seq &seed) override
    {
        return agent::act(step, seed);
    }
};


struct test_model
    : public esl::simulation::model
{
    using esl::simulation::model::model;

    int initialize_called = 0;

    int terminate_called = 0;

    void initialize() override
    {

        auto ta = this->template create<test_agent>();

        ++initialize_called;
    }

    void terminate() override
    {
        ++terminate_called;
    }
};

BOOST_AUTO_TEST_SUITE(ESL)

    BOOST_AUTO_TEST_CASE(environment_constructor)
    {
        esl::computation::environment e;

        test_model tm(e, esl::simulation::parameter::parametrization(0, 1, 100));

        BOOST_CHECK_EQUAL(tm.start, 1);
        BOOST_CHECK_EQUAL(tm.time, 1);
        BOOST_CHECK_EQUAL(tm.end, 100);
    }

    BOOST_AUTO_TEST_CASE(environment_basic_time_stepping)
    {
        esl::computation::environment e;

        test_model tm(e, esl::simulation::parameter::parametrization(0, 0, 100));

        auto next_ = tm.step( {0, 3});

        BOOST_CHECK_EQUAL(next_, 3);
    }


BOOST_AUTO_TEST_SUITE_END()  // ESL
