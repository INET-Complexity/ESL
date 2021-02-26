/// \file   python_module.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-08-30
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
#include <esl/computation/python_module_computation.hpp>


#ifdef WITH_PYTHON
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
using namespace boost::python;

#include <esl/simulation/model.hpp>

#include <esl/computation/block_pool.hpp>
#include <esl/computation/environment.hpp>
#include <esl/computation/timing.hpp>
using esl::computation::agent_timing;


namespace esl::computation {


    BOOST_PYTHON_MODULE(_computation)
    {
        // export block_pool memory block
        class_<block_pool::block<object>>("block")
            .def_readwrite("data", &block_pool::block<object>::data)
            .def_readwrite("index", &block_pool::block<object>::index)
            ;

        // computational environment base class with default single thread
        class_<python_environment>("environment")
            .def("step", &python_environment::step)
            .def("run", &python_environment::run)
            .def("activate", &python_environment::activate)
            .def("deactivate", &python_environment::deactivate)
            .def("before_step", &python_environment::before_step)
            .def("after_step", &python_environment::after_step)
            .def("after_run", &python_environment::after_run)
            .def("activate_agent", &python_environment::activate_agent)
            .def("deactivate_agent", &python_environment::deactivate_agent)
            .def("send_messages", &python_environment::send_messages)
            ;

        // timing information
        class_<agent_timing>("agent_timing")
            .def_readwrite("messaging", &agent_timing::messaging)
            .def_readwrite("acting", &agent_timing::acting);


    }


}

#endif