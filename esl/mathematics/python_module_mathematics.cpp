///// \file   python_module.cpp
/////
///// \brief
/////
///// \authors    Maarten P. Scholl
///// \date       2020-08-30
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
//#include <esl/mathematics/python_module_mathematics.hpp>
//
//
//#ifdef WITH_PYTHON
//
//#include <esl/mathematics/variable.hpp>
//
//#define BOOST_BIND_GLOBAL_PLACEHOLDERS
//#include <boost/python.hpp>
//using namespace boost::python;
//
//#include <adept/Stack.h>
//
//typedef adept::internal::BinaryOperation<double, adept::Active<double>, adept::internal::Add, adept::Active<double> > binary_operation_add_t;
//typedef adept::internal::BinaryOperation<double, adept::Active<double>, adept::internal::Subtract, adept::Active<double> > binary_operation_subtract_t;
//typedef adept::internal::BinaryOperation<double, adept::Active<double>, adept::internal::Multiply, adept::Active<double> > binary_operation_multiply_t;
//typedef adept::internal::BinaryOperation<double, adept::Active<double>, adept::internal::Divide, adept::Active<double> > binary_operation_divide_t;
//
//typedef adept::internal::BinaryOperation<double, adept::Active<double>, adept::internal::Pow, adept::Active<double> > binary_operation_pow_t;
//typedef adept::internal::BinaryOperation<double, adept::Active<double>, adept::internal::Atan2, adept::Active<double> > binary_operation_atan2_t;
//typedef adept::internal::BinaryOperation<double, adept::Active<double>, adept::internal::Max, adept::Active<double> > binary_operation_max_t;
//typedef adept::internal::BinaryOperation<double, adept::Active<double>, adept::internal::Min, adept::Active<double> > binary_operation_min_t;
//
//
//
//using namespace esl;
//
//
//BOOST_PYTHON_MODULE(_mathematics)
//{
//    class_<variable>( "variable", init<>())
//        .def(init<double>())
//        .def(self == self)
//        .def(self != self)
//        .def(self < self)
//        .def(self <= self)
//        .def(self > self)
//        .def(self >= self)
//        .def(self + self)
//        .def(self += self)
//        .def(self - self)
//        .def(self -= self)
//        .def(self * self)
//        .def(self *= self)
//        .def(self / self)
//        .def(self /= self)
//        //.def("value", &variable::value)
//        ;
//
//    class_<adept::Stack>("stack", init<>())
//        .def("pause_recording", &adept::Stack::pause_recording)
//        .def("continue_recording", &adept::Stack::continue_recording)
//        .def("new_recording", &adept::Stack::new_recording)
//        .def("compute_adjoint", &adept::Stack::compute_adjoint)
//    ;
//
//    // TODO: verify that all opeartions on this are exported
//    class_<binary_operation_add_t>("binary_operation_add_t", no_init)
//        .def(self == self)
//        .def(self != self)
//        .def(self < self)
//        .def(self <= self)
//        .def(self > self)
//        .def(self >= self)
//        .def(self + self)
//        .def(self - self)
//        .def(self * self)
//        .def(self / self)
//        ;
//
//    class_<binary_operation_subtract_t>("binary_operation_subtract_t", no_init)
//        .def(self == self)
//        .def(self != self)
//        .def(self < self)
//        .def(self <= self)
//        .def(self > self)
//        .def(self >= self)
//        .def(self + self)
//        .def(self - self)
//        .def(self * self)
//        .def(self / self)
//        ;
//
//    class_<binary_operation_multiply_t>("binary_operation_multiply_t", no_init)
//        .def(self == self)
//        .def(self != self)
//        .def(self < self)
//        .def(self <= self)
//        .def(self > self)
//        .def(self >= self)
//        .def(self + self)
//        .def(self - self)
//        .def(self * self)
//        .def(self / self)
//        ;
//    class_<binary_operation_divide_t>("binary_operation_divide_t", no_init)
//        .def(self == self)
//        .def(self != self)
//        .def(self < self)
//        .def(self <= self)
//        .def(self > self)
//        .def(self >= self)
//        .def(self + self)
//        .def(self - self)
//        .def(self * self)
//        .def(self / self)
//        ;
//// TODO: export these
////    def("get_gradients", &adept::get_gradients);
////    stack_.independent(&active_[0], active_.size());
////    stack_.dependent(&values_[0], values_.size());
////    stack_.jacobian(jacobian);
//
//
//
//}
//
//#endif