/// \file   test_owner.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-27
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
#define BOOST_TEST_MODULE owner


#include <boost/test/included/unit_test.hpp>


#include <esl/law/property.hpp>
#include <esl/economics/owner.hpp>
#include <esl/computation/environment.hpp>
#include <esl/simulation/model.hpp>


using namespace esl;


struct test_property_fungible
: public esl::law::property
, public esl::economics::fungible
{
    using esl::law::property::property;
};

struct test_property_infungible
: public esl::law::property
, public esl::economics::infungible
{
    using esl::law::property::property;
};

namespace std {
    template<>
    struct hash<test_property_fungible>
    {
        [[nodiscard]] constexpr std::size_t
        operator()(const test_property_fungible &p) const
        {
            (void)p;
            return 1234;// std::hash<esl::identity<esl::law::property>>(p.identifier);
        }
    };

    template<>
    struct hash<test_property_infungible>
    {
        [[nodiscard]] constexpr std::size_t
        operator()(const test_property_infungible &p) const
        {
            (void)p;
            return 3465;//std::hash<esl::identity<esl::law::property>>(p.identifier;
        }
    };
}




BOOST_AUTO_TEST_SUITE(ESL)

    struct owner_test
    : public esl::law::owner<esl::law::property>
    , public esl::identifiable_as<owner_test>
    {
        explicit owner_test(const esl::identity<owner<esl::law::property>> & i =
            esl::identity<esl::law::owner<esl::law::property>>())
            : agent(i)
            , owner<esl::law::property>(i)
        {

        }
    };

    struct test_model
    : public esl::simulation::model
    {
        test_model(esl::computation::environment &e, esl::simulation::parameter::parametrization parameters)
        : esl::simulation::model(e, parameters)
        {

        }

        void initialize() override
        {

        }

        ///
        ///
        ///
        esl::simulation::time_point step(esl::simulation::time_interval step) override
        {
            return model::step(step);
        }
    };




    struct owner_test_derived_properties
        : public virtual esl::law::owner<test_property_fungible>
        , public virtual esl::law::owner<test_property_infungible>
        , public esl::identifiable_as<owner_test_derived_properties>
    {
        explicit owner_test_derived_properties(
            const identity<owner_test_derived_properties> &i =
        identity<owner_test_derived_properties>())
        : agent(i)
        , esl::law::owner<test_property_fungible>(i)
        , esl::law::owner<test_property_infungible>(i)
        {

        }
    };


    BOOST_AUTO_TEST_CASE(owner_property_constructor)
    {
        esl::identity<esl::law::owner<esl::law::property>> i = {1,2,3};
        auto o = std::make_shared<esl::law::owner<esl::law::property>>(i);

        BOOST_CHECK_EQUAL(o->identifier, i);
    }

    BOOST_AUTO_TEST_CASE(owner_property_castable)
    {
        esl::identity<esl::law::owner<esl::law::property>> i = {1,2,3};
        auto o = std::make_shared<owner_test>(i);

        auto i_o = (identity<esl::law::owner<esl::law::property>>)(*o);
        BOOST_CHECK_EQUAL(i_o, i);
        auto i_t = (identity<owner_test>)(*o);
        BOOST_CHECK_EQUAL(i_t, i);
        auto i_a = (identity<agent>)(*o);
        BOOST_CHECK_EQUAL(i_a, i);


        BOOST_CHECK_EQUAL(o->inventory.size(), 0);
    }

    BOOST_AUTO_TEST_CASE(owner_property_message_callbacks)
    {
        computation::environment e;

        test_model tm(e, esl::simulation::parameter::parametrization(0, 0, 10));
        auto o_1 = tm.template create<owner_test>();
        auto o_2 = tm.template create<owner_test>();

        auto a0=(*tm.agents.local_agents_.begin()).first;

        esl::economics::accounting::inventory_filter<esl::law::property> iv_;


        auto p = std::make_shared<esl::law::property>(identity<esl::law::property>());
        iv_.insert(p, quantity(12300));

        auto test_message_ = std::make_shared<interaction::transfer>(
            *o_1, *o_2, *o_1, *o_2, iv_
            );


        o_1->inventory.insert({p , quantity(12300)});
        o_1->inbox.insert({0, test_message_});
        o_1->outbox.emplace_back(test_message_);

        BOOST_CHECK_EQUAL(o_1->inventory[p] , quantity(12300));

        tm.step({0, 10});

        BOOST_CHECK_EQUAL(o_2->inventory.size() , 0);

        tm.step({0, 10});

        BOOST_CHECK_EQUAL(o_2->inventory.size() , 1);
        BOOST_CHECK_EQUAL(o_2->inventory[p] , quantity(12300));

    }



    BOOST_AUTO_TEST_CASE(owner_property_message_callbacks_funbigility)
    {
        computation::environment e;
        test_model tm(e, esl::simulation::parameter::parametrization(0, 0, 10));
        auto o_1 = tm.template create<owner_test_derived_properties>();
        auto o_2 = tm.template create<owner_test_derived_properties>();
        auto a0=(*tm.agents.local_agents_.begin()).first;
        esl::economics::accounting::inventory_filter<esl::law::property> ivf_;
        esl::economics::accounting::inventory_filter<esl::law::property> ivi_;
        auto pf = std::make_shared<test_property_fungible>(identity<test_property_fungible>({1}));
        ivf_.insert(pf, quantity(23400));
        auto pi = std::make_shared<test_property_infungible>(identity<test_property_infungible>({2}));
        ivi_.insert(pi, quantity(1));

        auto test_message_f = std::make_shared<interaction::transfer>(
            *o_1, *o_2, *o_1, *o_2, ivf_
        );

        auto test_message_i = std::make_shared<interaction::transfer>(
            *o_1, *o_2, *o_1, *o_2, ivi_
        );

        o_1->inventory.insert({pf, quantity(23400)});
        o_1->inventory.insert({pi, quantity(1)});
        o_1->esl::law::owner<test_property_fungible>::properties.insert(pf, quantity(23400));
        o_1->esl::law::owner<test_property_infungible>::properties.insert(pi);

        o_1->inbox.insert({0, test_message_f});
        o_1->inbox.insert({0, test_message_i});
        o_1->outbox.emplace_back(test_message_f);
        o_1->outbox.emplace_back(test_message_i);


        BOOST_CHECK_EQUAL(o_1->inventory[pf] , quantity(23400));
        BOOST_CHECK_EQUAL(o_1->inventory[pi] , quantity(1));
        tm.step({0, 10});
        BOOST_CHECK_EQUAL(o_2->inventory.size() , 0);
        tm.step({0, 10});
        BOOST_CHECK_EQUAL(o_2->inventory.size() , 2);
        BOOST_CHECK_EQUAL(o_2->inventory[pf] , quantity(23400));
        BOOST_CHECK_EQUAL(o_2->inventory[pi] , quantity(1));
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL
