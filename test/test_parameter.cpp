/// \file   test_parameter.cpp
/// \brief
/// \authors    maarten
/// \date       2021-02-08
/// \copyright  Copyright 2017-2021 The Institute for New Economic Thinking,
///             Oxford Martin School, University of Oxford
///             Licensed under the Apache License, Version 2.0 (the "License");
///             you may not use this file except in compliance with the License.
///             You may obtain a copy of the License at
///                 http://www.apache.org/licenses/LICENSE-2.0
///             Unless required by applicable law or agreed to in writing,
///             software distributed under the License is distributed on an "AS
///             IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
///             express or implied. See the License for the specific language
///             governing permissions and limitations under the License.
///             You may obtain instructions to fulfill the attribution
///             requirements in CITATION.cff
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE model_parallel

#include <set>

#include <boost/test/included/unit_test.hpp>


#include <esl/simulation/parameter/sampler.hpp>
using namespace esl::simulation::parameter;
using namespace esl::simulation::parameter::sampler;




BOOST_AUTO_TEST_SUITE(ESL)

    ///
    /// \brief  This tests whether the grid sampler can generate points with
    ///         different densities in each dimension, whether it generates the
    ///         expected number of points and whether they are all unique.
    ///
    BOOST_AUTO_TEST_CASE(parameter_test_sampler_grid)
    {
        auto param0 = std::make_shared<interval<double>>(1.0, 5.0);
        auto param1 = std::make_shared<interval<double>>(-3, 3.0);
        auto param2= std::make_shared<interval<double>>(100, 200);

        std::map<std::shared_ptr<interval<double>>, unsigned int> p;
        p.emplace(param0, 2);
        p.emplace(param1, 5);
        p.emplace(param2, 3);
        auto samples_ = grid(p);

        BOOST_CHECK_EQUAL(samples_.size(), 2*5*3);

        std::set<std::tuple<double, double, double>> unique_;
        for(const auto &e: samples_){

            unique_.insert( std::make_tuple( e.find(param0)->second
                , e.find(param1)->second
                , e.find(param2)->second
            ) );
        }
        BOOST_CHECK_EQUAL(unique_.size(), 2*5*3);
    }


    BOOST_AUTO_TEST_CASE(parameter_test_sampler_hypercube)
    {
        auto param0 = std::make_shared<interval<double>>(1.0, 5.0);
        auto param1 = std::make_shared<interval<double>>(-3, 3.0);
        auto param2= std::make_shared<interval<double>>(100, 200);

        std::vector<std::shared_ptr<interval<double>>> p;
        p.emplace_back(param0);
        p.emplace_back(param1);
        p.emplace_back(param2);
        auto samples_ = latin_hypercube(p, 10);

        BOOST_CHECK_EQUAL(samples_.size(), 10 );

        std::set<std::tuple<double, double, double>> unique_;
        for(const auto &e: samples_){

            unique_.insert(std::make_tuple
                               ( e.find(param0)->second
                                   , e.find(param1)->second
                                   , e.find(param2)->second
                               ));
        }
        BOOST_CHECK_EQUAL(unique_.size(), 10);
    }

    BOOST_AUTO_TEST_CASE(parameter_test_sampler_orthogonal)
    {
        int dimensions = 8;
        int samples = 64;
        std::seed_seq seed = {1,2,3,4,5};
        int duplication  = 5;
        auto res = improved_hypercube_sampling (dimensions, samples, seed, duplication);
        for(int d = 0; d < dimensions; ++d) {
            std::set<unsigned int> ps;
            //std::cout << res[d][0];
            ps.insert(res[d][0]);
            for(int s = 1; s < samples; ++s) {
            //    std::cout << "," << res[d][s];
                ps.insert(res[d][s]);
            }
            BOOST_CHECK_EQUAL(ps.size(), samples);
            //std::cout << std::endl;
        }
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL





























