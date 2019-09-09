/// \file   time.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2017-12-07
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
#ifndef ESL_SIMULATION_TIME_HPP
#define ESL_SIMULATION_TIME_HPP

#include <cstdint>

#include <esl/mathematics/interval.hpp>

namespace esl::simulation {
    ///
    /// \brief  Specifies a specific point in time.
    ///
    /// \note must have a default constructor that defaults to the minimum
    ///       time_point value
    ///
    typedef std::uint64_t time_point;

    ///
    /// \brief a rational measure that denotes a distance from a time_point
    ///
    typedef std::uint64_t time_duration;

    ///
    /// \brief  An interval that has a specific starting event (time point), and
    ///         a limiting upper bound.
    ///
    typedef mathematics::interval<time_point, true, false>
        time_step;  // a fixed begin and endpoint, but duration need not be
                        // specified

}  // namespace esl

#endif  // ESL_SIMULATION_TIME_HPP
