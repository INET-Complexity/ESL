/// \file   test_mpi_environment_fallback.cpp
///
/// \brief  This tests a single-process MPI environment as a fallback option
///
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-03
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
#define BOOST_TEST_MODULE mpi_environment_fallback



#include <boost/test/included/unit_test.hpp>

#ifdef WITH_MPI

// intrude into the class for testing purposes
#define protected public
#define private public
#include <esl/computation/distributed/mpi_environment.hpp>
#undef private
#undef protected


using namespace esl;
using namespace esl::computation::distributed;


#include <esl/computation/environment.hpp>
#include <esl/economics/owner.hpp>
#include <esl/law/property.hpp>
#include <esl/simulation/model.hpp>

#include <test/shared_properties.hpp>

#include <test/shared_property_owner.hpp>

struct test_model
: public esl::simulation::model
{
    test_model(esl::computation::environment &e, esl::simulation::parameter::parametrization parameters)
    : esl::simulation::model(e, parameters)
    {}

    void initialize() override
    {}

    ///
    ///
    ///
    esl::simulation::time_point
    step(esl::simulation::time_interval step) override
    {
        return model::step(step);
    }
};



BOOST_AUTO_TEST_SUITE(ESL)

///
/// \brief  This is the owner test case, but now run over MPI.
///
/// \note   this test case runs on a single MPI process
///
BOOST_AUTO_TEST_CASE(initialization)
{
    mpi_environment e;

    test_model tm(e, esl::simulation::parameter::parametrization(0, 0, 10));
    auto o_1 = tm.template create<owner_test_derived_properties>();
    auto o_2 = tm.template create<owner_test_derived_properties>();
    auto a0  = (*tm.agents.local_agents_.begin()).first;
    esl::economics::accounting::inventory_filter<esl::law::property> ivf_;
    esl::economics::accounting::inventory_filter<esl::law::property> ivi_;
    auto pf = std::make_shared<test_property_fungible>(
        identity<test_property_fungible>({1}));
    ivf_.insert(pf, quantity(234, 100));
    auto pi = std::make_shared<test_property_infungible>(
        identity<test_property_infungible>({2}));
    ivi_.insert(pi, quantity(1, 1));

    auto test_message_f =
        std::make_shared<interaction::transfer>(*o_1, *o_2, *o_1, *o_2, ivf_);

    auto test_message_i =
        std::make_shared<interaction::transfer>(*o_1, *o_2, *o_1, *o_2, ivi_);

    o_1->inventory.insert({pf, quantity(234, 100)});
    o_1->inventory.insert({pi, quantity(1, 1)});
    o_1->esl::law::owner<test_property_fungible>::properties.insert(
        pf, quantity(234, 100));
    o_1->esl::law::owner<test_property_infungible>::properties.insert(pi);

    o_1->inbox.insert({0, test_message_f});
    o_1->inbox.insert({0, test_message_i});
    o_1->outbox.emplace_back(test_message_f);
    o_1->outbox.emplace_back(test_message_i);


    BOOST_CHECK_EQUAL(o_1->inventory[pf], quantity(234, 100));
    BOOST_CHECK_EQUAL(o_1->inventory[pi], quantity(1, 1));
    tm.step({0, 10});
    BOOST_CHECK_EQUAL(o_2->inventory.size(), 0);
    tm.step({0, 10});
    BOOST_CHECK_EQUAL(o_2->inventory.size(), 2);
    BOOST_CHECK_EQUAL(o_2->inventory[pf], quantity(234, 100));
    BOOST_CHECK_EQUAL(o_2->inventory[pi], quantity(1, 1));
}

BOOST_AUTO_TEST_SUITE_END()  // ESL


#endif WITH_MPI