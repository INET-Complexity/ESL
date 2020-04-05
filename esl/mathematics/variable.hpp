/// \file   variable.hpp
///
/// \brief  Wraps automatically differentiable variables.
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-04
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
#ifndef ESL_MATHEMATICS_VARIABLE_HPP
#define ESL_MATHEMATICS_VARIABLE_HPP

#include <adept.h>


namespace esl {
    typedef adept::adouble variable;

    template<typename t>
    inline double value(t value)
    {
        return double(value);
    }

    template<>
    inline double value(variable value)
    {
        return value.value();
    }

}  // namespace esl

#endif  // ESL_MATHEMATICS_VARIABLE_HPP
