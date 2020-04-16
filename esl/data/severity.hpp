/// \file   severity.hpp
///
/// \brief  Adds logging severity classes to the global scope.
///
/// \authors    Maarten P. Scholl
/// \date       2020-04-01
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
#ifndef ME_SEVERITY_HPP
#define ME_SEVERITY_HPP

#include <ostream>

namespace esl::data {

    enum severity
        : std::uint8_t
    {
        trace   =   0,
        notice  =   1,
        warning =   2,
        error   =   3
        //,
        //model
    };

    inline constexpr std::ostream &operator<<(std::ostream &stream, severity level)
    {
        switch(level) {
        case trace:
            return (stream << "trace");
        case notice:
            return (stream << "notice");
        case warning:
            return (stream << "warning");
        case error:
            return (stream << "error");
        }
        return stream;
    }
}

using namespace esl::data;


#endif  // ME_SEVERITY_HPP
