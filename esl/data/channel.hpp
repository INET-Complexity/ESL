/// \file   channel.hpp
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


#ifndef ME_CHANNEL_HPP
#define ME_CHANNEL_HPP

#include <vector>
#include <iostream>

#include <esl/data/severity.hpp>



namespace esl::data {
    ///
    /// \brief  A channel is a formatter that adds additional information
    ///         to the message. It is instantiated by the log class.
    ///
    struct channel
    {
        ///
        const severity level;

        ///
        std::ostream &stream;

        ///
        const char *function_name;
        const char *source_file;
        const unsigned int line;

        constexpr channel(severity level,
                          std::ostream &stream,
                          const char *function_name = nullptr,
                          const char *source_file   = nullptr,
                          unsigned int line         = 0)
        : level(level)
        , stream(stream)
        , function_name(function_name)
        , source_file(source_file)
        , line(line)
        { }

        template<typename output_t>
        channel &operator<<(output_t &&value)
        {
            std::vector<std::ostream *> os = {&stream};
#if !(defined(ESL_RELEASE) && ESL_RELEASE > 0)
            os.push_back(&std::cout);
#endif
            for(auto o : os) {
                (*o) << std::forward<output_t>(value);
            }
            return *this;
        }


        typedef channel &(*manipulator)(channel &);

        ///
        /// \param  function stream manipulator
        /// \return
        channel &operator<<(manipulator function)
        {
            return function(*this);
        }

        ///
        /// \brief
        /// \param  stream
        /// \return
        static channel &endl(channel &stream)
        {
            stream << std::endl;
#if !(defined(ESL_RELEASE) && ESL_RELEASE > 0)
            std::cout << std::endl;
#endif
            return stream;
        }

        typedef std::basic_ostream<char, std::char_traits<char>> ostream_t;

        // type of ostream manipulators such as std::endl
        typedef ostream_t &(*line_end_t)(ostream_t &);

        channel &operator<<(line_end_t manipulator)
        {
            manipulator(stream);
#if !(defined(ESL_RELEASE) && ESL_RELEASE > 0)
            manipulator(std::cout);
#endif
            return *this;
        }
    };
}

#endif  // ME_CHANNEL_HPP
