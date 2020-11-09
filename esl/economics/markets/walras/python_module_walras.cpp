/// \file   python_module_walras.cpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-10-11
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
#include <esl/economics/markets/walras/python_module_walras.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
#include <esl/economics/markets/walras/tatonnement.hpp>


#ifdef WITH_PYTHON
#include <boost/python.hpp>

using namespace boost::python;
using namespace esl::economics::markets;
using namespace esl::economics::markets::tatonnement;

BOOST_PYTHON_MODULE(_walras)
{
    enum_<excess_demand_model::solver>("solver")
        .value("root", excess_demand_model::root)
        .value("minimization", excess_demand_model::minimization)
        .export_values()
        ;

    class_<excess_demand_model>("excess_demand_model", init<esl::law::property_map<quote>>())
    .def_readwrite("circuit_breaker", &excess_demand_model::circuit_breaker)
        .def_readwrite("methods", &excess_demand_model::methods)
        .def_readwrite("quotes", &excess_demand_model::quotes)
        .def("compute_clearing_quotes", &excess_demand_model::compute_clearing_quotes)
        ;

}

#endif
