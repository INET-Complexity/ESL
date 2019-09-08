/// \file   identity.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-12-07
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
#include <esl/simulation/identity.hpp>


#ifdef WITH_PYTHON
#include <boost/python.hpp>

#include <vector>
using std::vector;

typedef esl::identity<boost::python::object> python_identity;

boost::shared_ptr<python_identity>
convert_digit_list(const boost::python::list &list)
{
    vector<uint64_t> result_;
    for(boost::python::ssize_t i = 0; i < boost::python::len(list); ++i) {
        result_.push_back(boost::python::extract<double>(list[i]));
    }
    return boost::make_shared<python_identity>(result_);
}

using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(python_identity_representation_overload,
                                       python_identity::representation, 0, 1);

BOOST_PYTHON_MODULE(identity)
{
    class_<python_identity, boost::noncopyable,
           boost::shared_ptr<python_identity>>("identity", no_init)
        .def("__init__", make_constructor(convert_digit_list))

        .def_readonly("digits", &python_identity::digits)

        .def("__str__", &python_identity::representation,
             python_identity_representation_overload(args("width"), ""))
        .def("__repr__", &python_identity::representation,
             python_identity_representation_overload(args("width"), ""))

        // clang-format off
        .def("__eq__", &python_identity::operator== <object>)
        .def("__ne__", &python_identity::operator!= <object>)
        .def("__lt__", &python_identity::operator<  <object>)
        .def("__le__", &python_identity::operator<= <object>)
        .def("__gt__", &python_identity::operator>  <object>)
        .def("__ge__", &python_identity::operator>= <object>);
    // clang-format on
}
#endif
