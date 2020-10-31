/// \file   python_module_distributed.cpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-10-16
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

#include <esl/computation/distributed/python_module_distributed.hpp>

#include <esl/computation/distributed/protocol.hpp>
using esl::computation::distributed::activation;
using esl::computation::distributed::migration;
using esl::computation::distributed::deactivation;


#ifdef WITH_PYTHON
#include <boost/python.hpp>

using namespace boost::python;
BOOST_PYTHON_MODULE(_distributed)
{
    scope().attr("__doc__") = "submodule for distributed computing using MPI";

    class_<activation>("activation")
        .def_readwrite("location", &activation::location)
        .def_readwrite("activated", &activation::activated);

    class_<migration>("migration")
        .def_readwrite("source", &migration::source)
        .def_readwrite("target", &migration::target)
        .def_readwrite("migrant", &migration::migrant);

    class_<deactivation>("deactivation")
        .def_readwrite("deactivated", &deactivation::deactivated);

}

#endif