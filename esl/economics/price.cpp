/// \file   price.cpp
///
/// \brief  The monetary value of something, as quoted in an offer, or agreed on in a transaction
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-28
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
#include <esl/economics/price.hpp>



#ifdef WITH_PYTHON
#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(price)
{
    class_<esl::economics::price>("price",
        init<std::int64_t, esl::economics::currency>())
        .def("value", &esl::economics::price::value)
        .def("valuation", &esl::economics::price::valuation)

        .def(self == self)
        .def(self != self)
        .def(self <  self)
        .def(self <= self)
        .def(self >  self)
        .def(self >= self)

        .def(self +  self)
        .def(self += self)
        .def(self -  self)
        .def(self -= self)

        .def(self * std::uint64_t)
        .def(std::uint64_t * self)


        ;
}

#endif  // WITH_PYTHON
