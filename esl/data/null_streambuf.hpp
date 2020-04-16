/// \file   null_streambuf.hpp
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


#ifndef ME_NULL_STREAMBUF_HPP
#define ME_NULL_STREAMBUF_HPP

#include <streambuf>


namespace esl::data {
///
    /// \brief  This class is a stand-in streambuffer that ignores any input
    ///         sent to it, so that logs can be switched on and off easily
    ///         with the compiler being able to recognise no side effects
    ///         take place and can decide to optimise it out
    ///
    class null_streambuf
    : public std::streambuf
    {
    public:
        /// \brief  This overrides C++ streambuf spec `xpsutn` with
        ///         a function that ignores the input. It is marked final
        ///         and (TODO) can be marked constexpr in C++20, so that the
        ///         compiler can optimise it out.
        ///
        /// \param n
        /// \return
        std::streamsize xsputn(const char *, std::streamsize n) final
        {
            return n;
        }

        /// \brief  This overrides C++ streambuf spec `overflow` with
        ///         a function that ignores the input and does not modify
        ///         any buffer. It is marked final
        //          and can be marked constexpr in C++20, so that the
        //          compiler can optimise it out.
        /// \param character
        /// \return
        int overflow(int character) final
        {
            return (character == EOF) ? '\0' : character;
        }
    };

}//namespace esl::data

#endif  // ME_NULL_STREAMBUF_HPP
