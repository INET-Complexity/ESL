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
#include <esl/simulation/python_module_simulation.hpp>


#ifdef WITH_PYTHON

#include <esl/computation/environment.hpp>

#include <esl/simulation/entity.hpp>
#include <esl/simulation/identity.hpp>
#include <esl/simulation/model.hpp>
#include <esl/simulation/time.hpp>
#include <esl/simulation/world.hpp>

#include <vector>
using std::vector;

#include <boost/make_shared.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
using namespace boost::python;


boost::shared_ptr<esl::simulation::python_module::python_identity>
convert_digit_list(const boost::python::list &list)
{
    vector<uint64_t> result_;
    for(boost::python::ssize_t i = 0; i < boost::python::len(list); ++i) {
        result_.push_back(boost::python::extract<std::uint64_t>(list[i]));
    }
    return boost::make_shared<esl::simulation::python_module::python_identity>(result_);
}



BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( python_identity_representation_overload
                                      , esl::simulation::python_module::python_identity::representation
                                      , 0
                                      , 1);

namespace esl::simulation {
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

    BOOST_PYTHON_MODULE(_simulation)
    {
        class_<esl::entity<object>, boost::noncopyable>(
            "entity", init<identity<object>>())
            .def_readonly("identifier", &esl::entity<object>::identifier)
            .def(self_ns::str(self_ns::self))
            .def("create", &esl::entity<object>::create<object>)
            .def(self == self)
            .def(self != self)
            ;

        class_<esl::simulation::python_module::python_identity>("identity")
            .def("__init__", make_constructor(convert_digit_list))
            .def_readonly("digits", &esl::simulation::python_module::python_identity::digits)
            .def("__str__", &esl::simulation::python_module::python_identity::representation,
                 python_identity_representation_overload(args("width"), ""))
            .def("__repr__", &esl::simulation::python_module::python_identity::representation,
                 python_identity_representation_overload(args("width"), ""))
            .def(self < self)
            .def(self > self)
            .def(self == self)
            .def(self != self)
            .def(self <= self)
            .def(self >= self)
            ;

       class_<model>("model"
                    ,init<esl::computation::environment &
                    , parameter::parametrization>()
                    )
           .def_readonly("start", &model::start)
           .def_readwrite("end", &model::end)
           .def_readwrite("time", &model::time);

       def("time_point", python_time_point);

       def("time_duration", python_time_duration);

       class_<time_interval>("time_interval",
                             init<time_point, time_point>())
           .def_readwrite("lower", &time_interval::lower)
           .def_readwrite("upper", &time_interval::upper)
           .def("empty", &time_interval::empty)
           .def("singleton", &time_interval::singleton)
           .def("degenerate", &time_interval::degenerate)
           .def("contains", &time_interval::contains)
           .def("__repr__", &time_interval::representation)
           .def("__str__", &time_interval::representation);

        /*
       class_<world, bases<world_entity>>("world", init<>())
           .def_readonly("identifier", &world::entity<world>::identifier)
           //.def("create", &world::entity<world>::identifier)
           .def("__repr__", &world::entity<world>::representation)
           ;

         implicitly_convertible<world, identity<world>>();
         */
    }
}

#endif