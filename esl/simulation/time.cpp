/// \file   time.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2017-12-07
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
#include <esl/simulation/time.hpp>
using namespace esl::simulation;

#ifdef WITH_PYTHON
#include <boost/python.hpp>
using namespace boost::python;


///
/// \brief  Export time_point constructor to Python.
///
/// \param o
/// \return
time_point python_time_point(const object& o)
{
    return extract<time_point>(o);
}


///
/// \brief  Export time_duration constructor to Python.
///
/// \param o
/// \return
time_duration python_time_duration(const object& o)
{
    return extract<time_duration>(o);
}


BOOST_PYTHON_MODULE(time)
{
    def("time_point",    python_time_point);

    def("time_duration", python_time_duration);

    class_<time_step>("time_step", init<time_point, time_point>())
        .def_readwrite("lower", &time_step::lower)
        .def_readwrite("upper", &time_step::upper)
        .def("empty",           &time_step::empty)
        .def("singleton",       &time_step::singleton)
        .def("degenerate",      &time_step::degenerate)
        .def("contains",        &time_step::contains)
        .def("__repr__",        &time_step::representation)
        .def("__str__",         &time_step::representation);
}

#endif
