/// \file   shared_property_owner.hpp
///
/// \brief  Simple property owner agents for testing purposes
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-04
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
#ifndef ESL_SHARED_PROPERTY_OWNER_HPP
#define ESL_SHARED_PROPERTY_OWNER_HPP

#include <esl/economics/owner.hpp>

struct test_property_owner
: public esl::law::owner<esl::law::property>
, public esl::identifiable_as<test_property_owner>
{
public:
    test_property_owner(const esl::identity<test_property_owner> &i =
                            esl::identity<test_property_owner>());

    ~test_property_owner() = default;

    template<class archive_t>
    void serialize(archive_t &archive, const unsigned int version)
    {
        (void)version;

        archive &boost::serialization::make_nvp(
            "owner⟨property⟩",
            boost::serialization::base_object<
                esl::law::owner<esl::law::property>>(*this));

        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(agent);
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(
            esl::law::owner<esl::law::property>);
    }
};


#include <test/shared_properties.hpp>

///
/// \brief  This agent has filters for a fungible and an infungible property,
///         in order to test that these filters work correctly
///
struct owner_test_derived_properties
: public virtual esl::law::owner<test_property_fungible>
, public virtual esl::law::owner<test_property_infungible>
, public esl::identifiable_as<owner_test_derived_properties>
{
    explicit owner_test_derived_properties(
        const esl::identity<owner_test_derived_properties> &i =
            esl::identity<owner_test_derived_properties>())
    : agent(i)
    , esl::law::owner<test_property_fungible>(i)
    , esl::law::owner<test_property_infungible>(i)
    {}

    template<class archive_t>
    void serialize(archive_t &archive, const unsigned int version)
    {
        (void)version;

        archive &boost::serialization::make_nvp(
            "owner⟨property⟩",
            boost::serialization::base_object<
                esl::law::owner<esl::law::property>>(*this));

        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(agent);
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(owner<esl::law::property>);
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(
            esl::law::owner<test_property_fungible>);
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(
            esl::law::owner<test_property_infungible>);
    }
};


#endif  // ESL_SHARED_PROPERTY_OWNER_HPP
