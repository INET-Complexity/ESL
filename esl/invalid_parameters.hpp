/// \file   invalid_parameters.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-10-12
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
#ifndef ESL_INVALID_PARAMETERS_HPP
#define ESL_INVALID_PARAMETERS_HPP

#include <esl/exception.hpp>

namespace esl {
    ///
    /// \brief  Exception raised when the market mechanism fails
    ///
    class invalid_parameters
    : esl::exception
    {
    public:
        explicit invalid_parameters(const std::string &message)
        : esl::exception(message)
        {

        }
    };
} // namespace esl

#endif  // ESL_INVALID_PARAMETERS_HPP
