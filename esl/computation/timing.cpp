/// \file   agent_timing.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-11-23
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
#include <esl/computation/timing.hpp>
using esl::computation::agent_timing;

#ifdef WITH_PYTHON
#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(timing)
{
    class_<agent_timing>("agent_timing")
        .def_readwrite("messaging", &agent_timing::messaging)
        .def_readwrite("acting", &agent_timing::acting);
}

#endif