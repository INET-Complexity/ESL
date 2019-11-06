/// \file   test_mpi_environment_fallback.cpp
///
/// \brief  This tests a multi-process MPI environment. In order to be properly
///         tested, this test case must be run on multiple nodes using an MPI
///         job runner (e.g. mpirun) in the desired computing environment, as
///         common bugs (mis-serialized pointers and uninitialized memory) may
///         go unnoticed on a single machine where processes share memory.
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


// intrude into the class for testing purposes

// firstly, include other headers, so they are unaffected


#ifdef WITH_MPI

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>

#include <unordered_map>

#include <esl/agent.hpp>
#include <esl/computation/distributed/protocol.hpp>
#include <esl/computation/environment.hpp>
#include <esl/simulation/identity.hpp>

// then, import the class itself
#define protected public
#define private public
#include <esl/computation/distributed/mpi_environment.hpp>
#undef private
#undef protected


using namespace esl;
using namespace esl::computation::distributed;


#include <esl/computation/environment.hpp>
#include <esl/simulation/model.hpp>

#include <boost/serialization/shared_ptr.hpp>


struct test_model : public esl::simulation::model
{
    test_model(esl::computation::environment &e,
               esl::simulation::parameter::parametrization parameters)
    : esl::simulation::model(e, parameters)
    {}

    void initialize() override
    {}

    ///
    esl::simulation::time_point
    step(esl::simulation::time_interval step) override
    {
        return model::step(step);
    }
};

///
/// \brief  We modify the class to test some things that would otherwise be
///         dependent on run-times, such as moving agents around. Only when we
///         can force agents to different processes, we can test interprocess
///         communication
///
class modified_mpi_environment
: public esl::computation::distributed::mpi_environment
{
public:
    using mpi_environment::mpi_environment;

    std::vector<migration> migrate_;
    ///
    ///
    /// \return
    virtual std::vector<migration> migrate_agents() override
    {
        auto result_ = std::vector<migration>(migrate_);
        migrate_.clear();
        return result_;
    }
};

#include <test/shared_property_owner.hpp>

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(test_property_fungible)
BOOST_CLASS_EXPORT(test_property_infungible)

typedef std::pair<test_property_infungible, esl::quantity> p0;
BOOST_CLASS_EXPORT(p0);
typedef std::pair<test_property_fungible, esl::quantity> p1;
BOOST_CLASS_EXPORT(p1);

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    modified_mpi_environment e;
    assert(e.communicator_.size() >= 2);
    test_model tm(e, esl::simulation::parameter::parametrization(0, 0, 10));
    if(e.communicator_.rank() == 0) {
        auto o_1    = tm.template create<owner_test_derived_properties>();
        auto o_2    = tm.template create<owner_test_derived_properties>();
        migration m = {0, 1, o_2->identifier};

        auto c = boost::dynamic_pointer_cast<esl::agent>(
            boost::make_shared<owner_test_derived_properties>(
                identity<owner_test_derived_properties>({1, 2, 3})));
        e.communicator_.send(1, 0, c);
        e.migrate_.push_back(m);


        esl::economics::accounting::inventory_filter<esl::law::property> ivf_;
        esl::economics::accounting::inventory_filter<esl::law::property> ivi_;
        auto pf = std::make_shared<test_property_fungible>(
            identity<test_property_fungible>({1}));

        ivf_.insert(pf, quantity(234, 100));
        auto pi = std::make_shared<test_property_infungible>(
            identity<test_property_infungible>({2}));
        ivi_.insert(pi, quantity(1, 1));


        auto test_message_f = std::make_shared<interaction::transfer>(
            *o_1, *o_2, *o_1, *o_2, ivf_);

        test_message_f->received = 4;

        auto test_message_i = std::make_shared<interaction::transfer>(
            *o_1, *o_2, *o_1, *o_2, ivi_);

        test_message_i->received = 4;


        o_1->inventory.insert({pf, quantity(234, 100)});
        o_1->inventory.insert({pi, quantity(1, 1)});
        o_1->esl::law::owner<test_property_fungible>::properties.insert(
            pf, quantity(234, 100));
        o_1->esl::law::owner<test_property_infungible>::properties.insert(pi);


        o_1->inbox.insert({0, test_message_f});
        o_1->outbox.emplace_back(test_message_f);

        o_1->inbox.insert({0, test_message_i});
        o_1->outbox.emplace_back(test_message_i);


    } else {
        boost::shared_ptr<esl::agent> c;
        e.communicator_.recv(0, 0, c);
    }

    tm.step({0, 10});
    if(e.communicator_.rank() == 1) {
        std::cout << "rank 1 now has " << tm.agents.local_agents_.size()
                  << std::endl;
        auto ptr = std::dynamic_pointer_cast<owner_test_derived_properties>(
            (*tm.agents.local_agents_.begin()).second);
        std::cout << ptr->inventory.size() << std::endl;
    }
    return 0;
}

#else

int main(int argc, char *argv[])
{
    return 0;
}

#endif