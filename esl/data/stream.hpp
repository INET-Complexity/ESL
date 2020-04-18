/// \file   sink.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-03-31
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
#ifndef ME_STREAM_HPP
#define ME_STREAM_HPP

#include <fstream>
#include <iostream>

namespace esl::data {
    class stream
    {
    protected:
        std::ostream &stream_;
    public:
        explicit stream(std::ostream &stream)
        : stream_(stream)
        {

        }

        operator std::ostream &()
        {
            return stream_;
        }
    };

    class terminal
    : public stream
    {
    public:
        enum type {
            out, error,
        };
    protected:
        const type type_;
    public:
        terminal(type standard = out)
        :   stream(standard == out ? std::cout : std::cerr)
        ,   type_(standard)
        {

        }
    };


    class file
    : public stream
    {
        std::ofstream file_;
    public:
        file(const std::string name)
        : stream(file_)
        , file_(name)
        {

        }
    };

    }

#endif  // ME_STREAM_HPP
