/// \file   test_legal_entity.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-01
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
#define BOOST_TEST_MODULE legal_entity

#include <boost/test/included/unit_test.hpp>

#include <esl/law/legal_entity.hpp>


BOOST_AUTO_TEST_SUITE(ESL)

BOOST_AUTO_TEST_CASE(legal_entity_examples)
{
    // Jaguar Land Rover Ltd
    // 2138-00-WSGIIZCXF1P5-72
    BOOST_CHECK_NO_THROW(esl::law::legal_entity("213800WSGIIZCXF1P572"));

    // BBC
    // 5493-00-0IBP32UQZ0KL-24
    std::array<char, 4> local_ = {'5', '4', '9', '3'};
    std::array<char, 12> code_ = {'0', 'I', 'B', 'P', '3', '2',
                                  'U', 'Q', 'Z', '0', 'K', 'L'};

    auto lei_      = esl::law::legal_entity(local_, code_);
    auto checksum_ = lei_.checksum();

    BOOST_CHECK_EQUAL(std::get<0>(checksum_), '2');
    BOOST_CHECK_EQUAL(std::get<1>(checksum_), '4');
}


BOOST_AUTO_TEST_CASE(legal_entity_representation)
{
    auto lei_ = esl::law::legal_entity("213800WSGIIZCXF1P572");
    std::stringstream stream_;

    stream_ << lei_;

    BOOST_CHECK_EQUAL(stream_.str(), "213800WSGIIZCXF1P572");
}

BOOST_AUTO_TEST_SUITE_END()  // ESL
