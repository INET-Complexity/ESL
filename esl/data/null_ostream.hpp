/// \file   null_ostream.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-04-16
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


#ifndef ME_NULL_OSTREAM_HPP
#define ME_NULL_OSTREAM_HPP

#include <ostream>

#include <esl/data/null_streambuf.hpp>


namespace esl::data {
    ///
    /// \brief  This class is a stand-in output stream that ignores any input
    ///         sent to it, so that logs can be switched on and off easily
    ///         with the compiler being able to recognise no side effects
    ///         take place and can decide to optimise it out
    class null_ostream
    : public std::ostream
    {
    protected:
        null_streambuf buffer_;

    public:
        null_ostream()
        : std::ostream(&buffer_)
        {

        }
    };
}

#endif  // ME_NULL_OSTREAM_HPP
