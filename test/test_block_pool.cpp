/// \file   test_block_pool.cpp
///
/// \brief
///
/// \authors    Maarten
/// \date       2020-06-27
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



#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE block_pool


#include <boost/test/included/unit_test.hpp>

#include <esl/computation/block_pool.hpp>


BOOST_AUTO_TEST_SUITE(ESL)

    BOOST_AUTO_TEST_CASE(block_pool_index)
    {
        esl::computation::block_pool::static_block_pool<int, 5> bp_;

        for(std::uint64_t i = 0; i < 5; ++i){
            auto a = bp_.emplace(i);
            BOOST_CHECK_EQUAL(a.first,i);
        }
        BOOST_CHECK_THROW( bp_.emplace(5), std::exception );

        std::cout << "bp_.size() " << bp_.size() << std::endl;
        bp_.erase(3);
        std::cout << "bp_.size() " << bp_.size() << std::endl;
        auto new_id_round_ = bp_.emplace(6);
        BOOST_CHECK_LT(5, new_id_round_.first);
        std::cout << "bp_.size() " << bp_.size() << std::endl;
        bp_.erase(0);
        std::cout << "bp_.size() " << bp_.size() << std::endl;
        bp_.erase(1);
        std::cout << "bp_.size() " << bp_.size() << std::endl;
        bp_.erase(2);
        std::cout << "bp_.size() " << bp_.size() << std::endl;
        bp_.erase(new_id_round_.first);
        std::cout << "bp_.size() " << bp_.size() << std::endl;
        bp_.erase(4);
        std::cout << "bp_.size() " << bp_.size() << std::endl;
        for(std::uint64_t i = 0; i < 5; ++i){
            auto a = bp_.emplace(i);
            (void)a;
            BOOST_CHECK_EQUAL(bp_.size(),i+1);
        }
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL
