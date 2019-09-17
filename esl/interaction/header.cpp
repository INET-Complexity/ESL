/// \file   header.cpp
///
/// \brief
///
/// \authors    maarten
/// \date       2019-09-11
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
#include <esl/interaction/header.hpp>


namespace esl {

    constexpr interaction::message_code
    python_library_message_code(interaction::message_code offset_)
    {
        // static_assert(message_mask >= offset_, "maximum code offset
        // exceeded");
        return interaction::message_mask | offset_;
    }

    constexpr interaction::message_code
    python_library_message_offset(interaction::message_code code_)
    {
        // static_assert(message_mask & code_, "not a library message code");
        return (~interaction::message_mask) & code_;
    }


}  // namespace esl

#ifdef WITH_PYTHON
#include <boost/python.hpp>
namespace esl::interaction {
    using namespace boost::python;
    BOOST_PYTHON_MODULE(header)
    {
        boost::python::scope().attr("message_mask") = message_mask;

        def("library_message_code", python_library_message_code);

        def("library_message_offset", python_library_message_offset);

        class_<esl::interaction::header>("header")
            .def_readonly("type", &header::type)
            .def_readwrite("sender", &header::sender)
            .def_readwrite("recipient", &header::recipient)
            .def_readwrite("sent", &header::sent)
            .def_readwrite("received", &header::received);
    }

}  // namespace esl::interaction
#endif
