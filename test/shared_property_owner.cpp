/// \file   shared_property_owner.cpp
///
/// \brief
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
#include <test/shared_property_owner.hpp>

#include <boost/serialization/export.hpp>
#if BOOST_VERSION >= 106500
#include <boost/serialization/unordered_map.hpp>
#endif

/// \param i
test_property_owner::test_property_owner(
const esl::identity<test_property_owner> &i)
: esl::agent(i), esl::law::owner<esl::law::property>(i)
{

}

BOOST_CLASS_EXPORT(test_property_owner)

BOOST_CLASS_EXPORT(owner_test_derived_properties)