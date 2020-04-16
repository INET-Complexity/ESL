/// \file   log.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-03-29
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
#ifndef ME_LOG_HPP
#define ME_LOG_HPP

#include <filesystem>
#include <iostream>

#include <boost/current_function.hpp>

#include <esl/data/null_ostream.hpp>
#include <esl/data/channel.hpp>
#include <esl/data/representation.hpp>
#include <esl/data/output.hpp>
#include <esl/simulation/time.hpp>


namespace esl::data {



    ///
    /// \brief  A logger is an output that collects human readable messages,
    ///         stored as strings, which are tagged with the message
    ///         priority. The messages can contain formatting strings
    ///         (provided by the {fmt} library) that can interpolate
    ///         a small number of predefined variables.
    ///
    /// \details    The named variables are:
    /// -#  {severity}          The message severity
    /// -#  {simulation_time}   The time point in the simulation
    /// -#  {message}           The message text
    /// -#  {timestamp}
    template<
//#if defined(ESL_RELEASE)
//        severity filter_ = warning
//#else
        severity filter_ = trace
//#endif
        >
    class log
    : public output_base
    {
    protected:
        std::ostringstream buffer_;
        null_ostream null_;

    public:
        const std::string format;

        log(const std::string &name, const std::string format = "({severity}) ")
        : output_base(name, {data::stream(buffer_),
                             data::terminal(data::terminal::type::out)})
        , format(format)
        {

        }

        ~log() override
        {

        }

        ///
        /// \brief  Returns the log channel for the specified severity.
        ///
        ///
        /// \details    This function uses a compile time argument so that
        ///             we can make a compile time decision to optimise away
        ///             the logging code.
        ///
        /// \tparam level_
        /// \return
        template<severity level_ = warning>
        constexpr channel get(const char *function_name = nullptr,
                              const char *source_file   = nullptr,
                              unsigned int line         = 0)
        {
            if(level_ < filter_) {
                return channel(level_, null_, format, function_name,
                               source_file, line);
            }
            auto channel_ = channel(level_,
                                    buffer_,
                                    format,
                                    function_name,
                                    source_file,
                                    line);
            if(source_file) {
                std::stringstream library_root_;
                library_root_
                    << "esl"
                    << char(std::filesystem::path::preferred_separator);
                const std::string root_ = library_root_.str();

                auto source_file_ = std::string(source_file);

                size_t common_part_ = source_file_.rfind(root_);
                if(std::string::npos == common_part_) {
                    common_part_ = 1 + source_file_.rfind(
                            char(std::filesystem::path::preferred_separator));
                    if(std::string::npos == common_part_) {
                        common_part_ = 0;
                    }
                }else{
                    common_part_ += root_.size();
                }
                channel_ << '(' << level_ << ", " << &source_file[common_part_]
                        << '[' << line << ']' << ')' << ' ';
            }else{
                channel_ << '(' << level_ << ')' << ' ';
            }
            return channel_;
        }

        ///
        /// \brief  Returns the log channel for the specified severity.
        ///
        /// \details    Prefer get<level>() whenever the message severity
        ///             level can be determined at compile time.
        ///
        /// \param level
        /// \return
        constexpr channel get(severity level)
        {
            if(level < filter_) {
                return channel(level, null_, format);
            }
            return channel(level, buffer_, format);
        }
    };


    ///
    /// \brief  The main log is always instantiated.
    ///
    static data::log main_log("main");
}  // namespace esl::data

#define LOG(level) \
    esl::data::main_log.get<level>(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__)

#endif  // ME_LOG_HPP
