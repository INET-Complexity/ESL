/// \file   message.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
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
#include <esl/interaction/message.hpp>

#include <boost/serialization/export.hpp>



/*
#ifdef WITH_PYTHON
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>

BOOST_CLASS_EXPORT(esl::interaction::python_message)

namespace esl::interaction {
    using namespace boost::python;

    BOOST_PYTHON_MODULE(message)
    {
        class_<python_message, bases<header>>("message")
            .def_readonly("code", &python_message::python_code);
    }

}  // namespace esl::interaction
#endif

*/