/// \file   test_model_parallel.cpp
///
/// \brief
///
/// \authors    maarten
/// \date       2021-02-06
/// \copyright  Copyright 2017-2021 The Institute for New Economic Thinking,
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
#define BOOST_TEST_MODULE model_parallel

#include <boost/test/included/unit_test.hpp>


#include <esl/simulation/identifiable_as.hpp>
#include <esl/simulation/identity.hpp>

#include <string>
using std::string;
#include <thread>


BOOST_AUTO_TEST_SUITE(ESL)

    BOOST_AUTO_TEST_CASE(model_test_parallel)
    {
        unsigned int concurrency_ = 0;
        // TODO
        BOOST_CHECK_NO_THROW( concurrency_ = std::thread::hardware_concurrency());
    }



BOOST_AUTO_TEST_SUITE_END()  // ESL
