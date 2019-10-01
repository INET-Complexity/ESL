/// \file   tangibility.cpp
///
/// \brief  Denotes whether or not an item is physical, and therefore must
///         occupy a location in the simulated world.
///         Also provides an interface to add additional attributes to physical
///         objects.
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-28
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
#ifndef ESL_TANGIBILITY_HPP
#define ESL_TANGIBILITY_HPP

#include <boost/serialization/serialization.hpp>

#include <esl/geography/location.hpp>

namespace esl::economics {
    ///
    /// \brief  A tangible is a physical object and its physical location and
    ///         state play an economic role e.g. transportation and
    ///         storage cost. It is not necessarily infungible; depending on the
    ///         buyers views, one lot can be as good as another.
    ///
    struct tangible
    {
        /// \brief
        geography::location place;
    };

    struct intangible
    {

    };
}  // namespace esl::economics

#endif  // ESL_TANGIBILITY_HPP
