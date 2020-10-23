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


using namespace esl;
using namespace esl::simulation;

struct test_agent
: public agent
{
    using agent::agent;

    unsigned int delay = 0;

    time_point act(time_interval step,
                                    std::seed_seq &seed) override
    {
        (void) seed;
        return step.lower + delay;
    }
};


struct test_model
    : public model
{
    using model::model;

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
        computation::environment e;

        test_model tm(e, parameter::parametrization(0, 1, 100));

        BOOST_CHECK_EQUAL(tm.start, 1);
        BOOST_CHECK_EQUAL(tm.time, 1);
        BOOST_CHECK_EQUAL(tm.end, 100);
    }

    BOOST_AUTO_TEST_CASE(environment_basic_time_stepping)
    {
        computation::environment e;
        test_model tm(e, parameter::parametrization(0, 0, 100));

        auto next_ = tm.step( {0, 3});

        std::cout << next_ << std::endl;

        BOOST_CHECK_EQUAL(next_, 3);
    }


    BOOST_AUTO_TEST_CASE(environment_time_step_with_agents)
    {
        computation::environment e;
        test_model tm(e, parameter::parametrization(0, 0, 100));

        auto a1 = tm.create<test_agent>();
        a1->delay = 5;

        auto next_ = tm.step( {0, 3});
        BOOST_CHECK_EQUAL(next_, 3);

        next_ = tm.step( {3, 5});

        BOOST_CHECK_EQUAL(next_, 5);
    }

    ///
    /// \brief
    ///
    BOOST_AUTO_TEST_CASE(environment_run_agents_parallel)
    {
        computation::environment e;
        unsigned int threads = 8;

        test_model tm(e, parameter::parametrization(0, 0, 100, 0, threads));

        // We create one agent with delay 5, and other agents have more.
        // If the parallelisation fails, we might miss that particular agent
        // and end with a higher time point although this is nondeterministic
        // and we are thus not guaranteed to catch it.
        auto test_agents = 100'000;
        for(auto i = 0; i < test_agents; ++i){
            auto a1 = tm.create<test_agent>();
            a1->delay = 4 + test_agents - i;
        }

        auto next_ = tm.step( {0, 3});
        BOOST_CHECK_EQUAL(next_, 3);

        // agents will delay again
        next_ = tm.step( {3, 1234});
        BOOST_CHECK_EQUAL(next_, 8);
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL
