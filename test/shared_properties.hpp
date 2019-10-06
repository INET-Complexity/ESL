/// \file   shared_properties.hpp
///
/// \brief  Plain properties for testing
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
#ifndef ESL_SHARED_PROPERTIES_HPP
#define ESL_SHARED_PROPERTIES_HPP

#include <esl/law/property.hpp>
#include <esl/economics/fungibility.hpp>

struct test_property_fungible
: public esl::law::property
, public esl::economics::fungible
{
    using esl::law::property::property;

    template<class archive_t>
    void serialize(archive_t &archive, const unsigned int version)
    {
        (void)version;
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(esl::law::property);
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(esl::economics::fungible);
    }
};

struct test_property_infungible
: public esl::law::property
, public esl::economics::infungible
{
    using esl::law::property::property;


    template<class archive_t>
    void serialize(archive_t &archive, const unsigned int version)
    {
        (void)version;
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(esl::law::property);
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(
            esl::economics::infungible);
    }
};

namespace std {
    template<>
    struct hash<test_property_fungible>
    {
        [[nodiscard]] constexpr std::size_t
        operator()(const test_property_fungible &p) const
        {
            (void)p;
            return std::hash<esl::identity<esl::law::property>>()(p.identifier);
        }
    };

    template<>
    struct hash<test_property_infungible>
    {
        [[nodiscard]] constexpr std::size_t
        operator()(const test_property_infungible &p) const
        {
            (void)p;
            return std::hash<esl::identity<esl::law::property>>()(p.identifier);
        }
    };
}  // namespace std

#ifdef WITH_MPI
#include <boost/mpi/datatype.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<test_property_fungible>
        : public mpl::false_
    {};

    template<>
    struct is_mpi_datatype<test_property_infungible>
        : public mpl::false_
    {};

}  // namespace boost::mpi
#endif  // WITH_MPI



#endif  // ESL_SHARED_PROPERTIES_HPP
