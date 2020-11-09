/// \file   loan.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-29
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

#include <esl/economics/finance/loan.hpp>
using namespace esl::economics::finance;

/*
#ifdef WITH_PYTHON
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(loan)
{
    class_<loan, bases<esl::law::contract>>
        ( "loan", init<esl::identity<esl::agent>, esl::identity<esl::agent>>())
        .add_property("lender"
            , make_function( &loan::lender
            , return_value_policy<copy_const_reference>())
            )
        .add_property("borrower"
            , make_function(&loan::borrower
            , return_value_policy<copy_const_reference>())
            )
        ;
}

#endif  // WITH_PYTHON
*/