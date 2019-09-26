/// \file   quantity.cpp
///
/// \brief  A class used to enforce conversion without numerical error when
///         counting property, goods and other items.
///
/// \authors    Maarten P. Scholl
/// \date       2018-02-05
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
#include <esl/quantity.hpp>


#ifdef WITH_PYTHON
#include <boost/python.hpp>

namespace esl {
    using namespace boost::python;

    BOOST_PYTHON_MODULE(quantity)
    {
        class_<quantity>(
            "agent",init<>())
            .def(init<std::uint64_t>())
            .def(init<std::uint64_t, std::uint64_t>())
            .def(self += self)
            .def(self + self)
            .def(self -= self)
            .def(self - self)
            //.def(self *= self)
            .def(self * self)
            //.def(self /= self)
            //.def(self / self)
            //.def(self %= self)
            //.def(self % self)

            // conversion to floating point type
            .def(float_(self))

            .def(self *= std::uint64_t())
            .def(self * std::uint64_t())
            .def(self / std::uint64_t())
            ;
    }
}  // namespace esl

#endif  // WITH_PYTHON