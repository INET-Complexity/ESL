/// \file   test_communicator.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-18
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
#define BOOST_TEST_MODULE communicator

#include <boost/test/included/unit_test.hpp>



#define private public
#define protected public
#include <esl/interaction/communicator.hpp>
#undef private
#undef protected

#include <esl/interaction/message.hpp>

#include <functional>

BOOST_AUTO_TEST_SUITE(ESL)

struct dummy_message
: public esl::interaction::message<dummy_message, (std::uint64_t(0x1) << 62u) | 0>
{

};

struct dummy_message_2
: public esl::interaction::message<dummy_message_2, (std::uint64_t(0x1) << 62u) | 1>
{

};

///
/// \brief  This testing class defines some callbacks and tracks the order in
///         which they are called
///
struct initializes_callbacks : public esl::interaction::communicator
{
    std::vector<int> execution_sequence;

    communicator::message_t p =
        std::static_pointer_cast<esl::interaction::header>(
            std::make_shared<dummy_message>());


    esl::simulation::time_point
    callback_test_function_high_priority(std::shared_ptr<dummy_message> m)
    {
        (void)m;
        execution_sequence.push_back(789);
        return esl::simulation::time_point(789);
    }

    esl::simulation::time_point
    callback_test_function(std::shared_ptr<dummy_message> m)
    {
        (void)m;
        execution_sequence.push_back(456);
        return esl::simulation::time_point(456);
    }


    esl::simulation::time_point
    callback_test_function_other(std::shared_ptr<dummy_message_2> m)
    {
        (void)m;
        execution_sequence.push_back(234);
        return esl::simulation::time_point(234);
    }

    initializes_callbacks()
    : esl::interaction::communicator(
        esl::interaction::communicator::scheduling::in_order)
    {
        this->register_callback<dummy_message>(
            [=](auto m, esl::simulation::time_interval s, std::seed_seq &seed) {
                (void) s;
                (void)seed;
                return callback_test_function(m);
            });

        this->register_callback<dummy_message>(
            [=](auto m, esl::simulation::time_interval s, std::seed_seq &seed) {
                (void) s;

                (void)seed;
                return callback_test_function_high_priority(m);
            },
            10);


        this->register_callback<dummy_message_2>(
            [=](auto m, esl::simulation::time_interval s, std::seed_seq &seed) {
                (void) s;
                (void)seed;
                return callback_test_function_other(m);
            },
            0);
    }
};

BOOST_AUTO_TEST_CASE(communicator_constructor)
{
    esl::interaction::communicator c;

    initializes_callbacks ic;

    BOOST_CHECK_EQUAL(ic.callbacks_.size(), 2);
    BOOST_CHECK_EQUAL(ic.callbacks_.find((std::uint64_t(0x1) << 62u) | 0)->second.size(), 2);
    BOOST_CHECK_EQUAL(ic.callbacks_.find((std::uint64_t(0x1) << 62u) | 1)->second.size(), 1);
}

BOOST_AUTO_TEST_CASE(communicator_process_single_message)
{
    initializes_callbacks ic;
    esl::simulation::time_interval step_ = {0, 999};
    std::seed_seq seed_;
    auto dm = std::make_shared<dummy_message>();
    auto r  = ic.process_message(dm, step_, seed_);

    BOOST_CHECK_EQUAL(r, 456);

    auto dm2 = std::make_shared<dummy_message_2>();
    auto r2  = ic.process_message(dm2, step_, seed_);

    BOOST_CHECK_EQUAL(r2, 234);
}


BOOST_AUTO_TEST_CASE(communicator_process_queue)
{
    initializes_callbacks ic;
    esl::simulation::time_interval step_ = {2, 999};

    for(size_t i = 0; i < 3; ++i) {
        auto dm = std::make_shared<dummy_message>();
        ic.inbox.insert({esl::simulation::time_point(i), dm});
        auto dm2 = std::make_shared<dummy_message_2>();
        ic.inbox.insert({esl::simulation::time_point(i), dm2});
    }

    std::seed_seq sequence_ {1};

    ic.process_messages(step_, sequence_);

    std::vector<int> expected_ = {789, 456, 789, 456, 789, 456, 234, 234, 234};

    auto result_ =
        std::equal(ic.execution_sequence.begin(), ic.execution_sequence.end(),
                   expected_.begin(), expected_.end());

    BOOST_CHECK(result_);
}

BOOST_AUTO_TEST_SUITE_END()  // ESL
