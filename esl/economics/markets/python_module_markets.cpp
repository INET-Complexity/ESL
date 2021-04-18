///// \file   python_module_markets.cpp
/////
///// \brief
/////
///// \authors    Maarten P. Scholl
///// \date       2020-11-20
///// \copyright  Copyright 2017-2020 The Institute for New Economic Thinking,
/////             Oxford Martin School, University of Oxford
/////
/////             Licensed under the Apache License, Version 2.0 (the "License");
/////             you may not use this file except in compliance with the License.
/////             You may obtain a copy of the License at
/////
/////                 http://www.apache.org/licenses/LICENSE-2.0
/////
/////             Unless required by applicable law or agreed to in writing,
/////             software distributed under the License is distributed on an "AS
/////             IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
/////             express or implied. See the License for the specific language
/////             governing permissions and limitations under the License.
/////
/////             You may obtain instructions to fulfill the attribution
/////             requirements in CITATION.cff
/////
//#include "python_module_markets.hpp"
//
//#ifdef WITH_PYTHON
//#define BOOST_BIND_GLOBAL_PLACEHOLDERS
//#include <boost/python.hpp>
//#include <boost/python/overloads.hpp>
//using namespace boost::python;
//
//#include <esl/economics/markets/iso_10383.hpp>
//#include <esl/economics/markets/ticker.hpp>
//#include <esl/economics/markets/market.hpp>
//using namespace esl;
//using namespace esl::economics;
//using namespace esl::economics::markets;
//
//#include <esl/economics/markets/quote.hpp>
//
//price quote_helper_get_price(const quote &q)
//{
//    auto p = std::get_if<price>(&q.type);
//    return *p;
//}
//
//
//void quote_helper_set_price(quote &q, price p)
//{
//    q.type = p;
//}
//
//double quote_helper_operator_double(const quote &q)
//{
//    return double(q);
//}
//
//
////float quote_helper_operator_float(const quote &q)
////{
////    return float(q);
////}
//
//
//boost::shared_ptr<quote> construct_quote_from_price(price p)
//{
//    return boost::make_shared<quote>(p);
//}
//
//
//boost::shared_ptr<quote> construct_quote_from_exchange_rate(const exchange_rate &e)
//{
//    return boost::make_shared<quote>(e);
//}
//
//BOOST_PYTHON_MODULE(_markets)
//{
//    ////////////////////////////////////////////////////////////////////////////
//    class_<iso_10383>("iso_10383", init<std::string>() )
//        .def("__repr__", &iso_10383::representation)
//        .def(self_ns::str(self_ns::self))
//        .def(self == self)
//        .def(self != self)
//        .def(self < self)
//        .def(self > self)
//        .def(self <= self)
//        .def(self >= self)
//        ;
//
//    ////////////////////////////////////////////////////////////////////////////
//
//    class_<quote, boost::noncopyable, boost::shared_ptr<quote> >("quote", no_init)
//        .def("__init__", boost::python::make_constructor(construct_quote_from_price))
//        .def("__init__", boost::python::make_constructor(construct_quote_from_exchange_rate))
//        .add_property("price", &quote_helper_get_price, &quote_helper_set_price)
//        .def_readwrite("lot", &quote::lot)
//
//        .def(self == self)
//        .def(self != self)
//        .def(self < self)
//        .def(self > self)
//        .def(self <= self)
//        .def(self >= self)
//
//        .def("__float__", &quote_helper_operator_double)
//
//        .def("__repr__", &quote::representation)
//        .def("__str__", &quote::representation)
//        ;
//
//    implicitly_convertible<quote, double>();
//
//    ////////////////////////////////////////////////////////////////////////////
//    class_<ticker>( "ticker"
//                  , init<identity<law::property>, identity<law::property>>()
//                  )
//        .def_readwrite("base", &ticker::base)
//        .def_readwrite("quote", &ticker::quote)
//        .def(self == self)
//        .def(self != self)
//        .def(self < self)
//        .def(self > self)
//        .def(self <= self)
//        .def(self >= self)
//        ;
//
//    ////////////////////////////////////////////////////////////////////////////
//    enum_<indication>("indication")
//        .value("firm", indication::firm)
//        .value("indicative", indication::indicative)
//    ;
//
//    ////////////////////////////////////////////////////////////////////////////
//    //class_<python_quote_message>("quote_message", )
//
//    //class_<market>("market", init<...>)
//
//}
//
//#endif