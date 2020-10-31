/// \file   delimiter_separated_values.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-01-31
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
#ifndef ME_DELIMITER_SEPARATED_VALUES_HPP
#define ME_DELIMITER_SEPARATED_VALUES_HPP

#include <ostream>
#include <sstream>
#include <tuple>
#include <vector>


namespace esl::data::format::delimiter_separated_values {

    ///
    /// \brief  Renders a string with DSV escape characters
    ///
    /// \param text
    /// \param delimiter
    /// \param quote
    /// \param escape
    /// \return
    std::string render( const std::string &text
                      , const char delimiter = ','
                      , const char quote = '"'
                      , const char escape = '\\'
                      )
    {
        bool quote_ = false;
        if(text.find(delimiter) != std::string::npos) {
            quote_ = true;
        } else if(text.find(escape) != std::string::npos) {
            quote_ = true;
        }

        std::stringstream stream_;
        if(quote_) {
            stream_ << quote;
        }

        for(auto c: text) {
            if(quote == c || escape == c) {
                stream_ << escape;
            }
            stream_ << c;
        }

        if(quote_) {
            stream_ << quote;
        }
        return stream_.str();
    }


    template<typename element_t_>
    void extract_element(std::ostream &s, element_t_ v)
    {
        s << v;
    }

    ///
    /// \brief  Specialization for strings, which must be escaped.
    ///
    /// \param s
    /// \param v
    template<>
    void extract_element(std::ostream &s, const std::string &v)
    {
        s << render(v);
    }


    template<typename... elements_t_>
    static void extract_element( std::ostream &s
                               , std::string &&first
                               , elements_t_ &&... rest)
    {
        char delimiter_ = ',';
        s << render(first) << delimiter_;
        extract_element(s, std::forward<elements_t_>(rest)...);
    }

    template<typename element_t_, typename... elements_t_>
    static void extract_element(std::ostream &s
                               , element_t_ &&first
                               , elements_t_ &&... rest)
    {
        char delimiter_ = ',';
        s << first << delimiter_;
        extract_element(s, std::forward<elements_t_>(rest)...);
    }

    ///
    /// \tparam element_t_
    /// \param stream
    /// \param tuple
    /// \return
    template<typename... element_t_>
    void render_row(std::ostream &stream,
                    const std::tuple<element_t_...> &tuple)
    {
        auto print_ = [&](auto... elements_) {
            return extract_element(stream, elements_...);
        };
        std::apply(print_, tuple);
        stream << std::endl;
    }

    ///
    /// \tparam element_t_
    /// \param stream
    /// \param tuple
    /// \return
    template<typename... element_t_>
    void render_records(std::ostream &stream,
                        const std::vector<std::tuple<element_t_...>> &records)
    {
        for(const auto &record_: records) {
            render_row(stream, record_);
        }
    }

}  // namespace esl::data::format::delimiter_separated_values

#endif  // ME_DELIMITER_SEPARATED_VALUES_HPP
