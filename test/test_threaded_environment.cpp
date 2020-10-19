/// \file   test_threaded_environment.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-10-18
/// \copyright  Copyright 2017-2020 The Institute for New Economic Thinking,
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
/// \file   test_mpi_environment_fallback.cpp
///
/// \brief  This tests a multi-process MPI environment. In order to be properly
///         tested, this test case must be run on multiple nodes using an MPI
///         job runner (e.g. mpirun) in the desired computing environment, as
///         common bugs (mis-serialized pointers and uninitialized memory) may
///         go unnoticed on a single machine where processes share memory.
///

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE threaded_environmnet

#include <boost/test/included/unit_test.hpp>

#include <unordered_map>
#include <esl/agent.hpp>

#define protected public
#define private public
#include <esl/computation/distributed/threaded_environment.hpp>
#undef private
#undef protected


using namespace esl;
using namespace esl::computation::distributed;


#include <esl/computation/environment.hpp>
#include <esl/simulation/model.hpp>

#include <boost/serialization/shared_ptr.hpp>

///
/// \brief  Testing model used to test MPI environment.
///
struct test_model
    : public esl::simulation::model
{
    test_model(esl::computation::environment &e,
               esl::simulation::parameter::parametrization parameters)
        : esl::simulation::model(e, parameters)
    {

    }

    void initialize() override
    {

    }

    ///
    esl::simulation::time_point
    step(esl::simulation::time_interval step) override
    {
        return model::step(step);
    }
};



BOOST_AUTO_TEST_SUITE(ESL)

BOOST_AUTO_TEST_CASE(threaded_environment)
{
    esl::computation::distributed::threaded_environment environment_;


}


BOOST_AUTO_TEST_SUITE_END()  // ESL



