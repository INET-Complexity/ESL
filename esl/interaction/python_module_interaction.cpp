/// \file   python_module_interaction.cpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-11-27
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
#include <esl/interaction/python_module_interaction.hpp>
#include <esl/interaction/transfer.hpp>
using namespace esl;
using namespace esl::simulation;
using namespace esl::interaction;


#ifdef WITH_PYTHON
#include <string>

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(esl::interaction::python_message);


BOOST_PYTHON_MODULE(_interaction)
{
    def("make_callback_handle", &make_callback_handle);

    class_<communicator::callback_handle>( "callback_handle");

    class_<communicator::callback_t>( "callback_t")
        .def_readwrite("function", &communicator::callback_t::function)
        .def_readwrite("description", &communicator::callback_t::description)
        .def_readwrite("message", &communicator::callback_t::message)
        .def_readwrite("file", &communicator::callback_t::file)
        .def_readwrite("line", &communicator::callback_t::line)
        //.def("__call__", &communicator::callback_t::operator ())
        ;

    class_<communicator>("communicator")
        .def_readonly("code", &python_message::python_code)
        ;

    class_<header>("header", init<message_code, identity<agent>, identity<agent>, simulation::time_point, simulation::time_point>())
        .def(init<message_code, identity<agent>, identity<agent>, simulation::time_point>())
        .def(init<message_code, identity<agent>, identity<agent>>())
        .def(init<message_code, identity<agent>>())
        .def(init<message_code>())
        .def(init<message_code>())
        .def_readwrite("type", &header::type)
        .def_readwrite("sender", &header::sender)
        .def_readwrite("recipient", &header::recipient)
        .def_readwrite("sent", &header::sent)
        .def_readwrite("received", &header::received)
    ;

    class_<python_message, bases<header>>("message")
        .def_readonly("code", &python_message::python_code)
    ;
}

#endif