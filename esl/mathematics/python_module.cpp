/// \file   python_module.cpp
///
/// \brief
///
/// \authors    maarten
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
#include <esl/mathematics/python_module.hpp>


#ifdef WITH_PYTHON

#include <esl/mathematics/variable.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
using namespace boost::python;


namespace esl::mathematics {
    BOOST_PYTHON_MODULE(mathematics)
    {
        class_<variable>( "variable",init<>())
            .def(init<std::uint64_t>())
            .def(init<double>())
            .def(self + self)
            .def(self += self)
            .def(self - self)
            .def(self -= self)
            .def(self * self)
            .def(self *= self)
            .def(self / self)
            .def(self /= self)
//            .def(self % self)
//            .def(self %= self)
//            .def(float_(self))
            .def("value", &variable::value)
            ;

        ///
        /// Overload only the specialization, because the other one is merely
        /// a cast and we can do that in Python also.
        ///
        def("value", value<variable>);
    }
}

#endif