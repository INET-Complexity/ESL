/// \file   python_module_markets.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-11-20
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
#include "python_module_markets.hpp"

#ifdef WITH_PYTHON
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
using namespace boost::python;

#include <esl/economics/markets/iso_10383.hpp>
//#include <esl/economics/markets/order_book/basic_book.hpp>
//#include <esl/economics/markets/order_book/binary_tree_order_book.hpp>
//#include <esl/economics/markets/order_book/static_order_book.hpp>
//#include <esl/economics/markets/order_book/matching_engine.hpp>
//#include <esl/economics/markets/order_book/python_module_order_book.hpp>
using namespace esl::economics;
using namespace esl::economics::markets;
//using namespace esl::economics::markets::order_book;



BOOST_PYTHON_MODULE(_markets)
{
    class_<iso_10383>("iso_10383", init<std::string>() )
        .def("__repr__", &iso_10383::representation)
        .def(self_ns::str(self_ns::self))
        .def(self == self)
        .def(self != self)
        .def(self < self)
        .def(self > self)
        .def(self <= self)
        .def(self >= self)
        ;
}

#endif