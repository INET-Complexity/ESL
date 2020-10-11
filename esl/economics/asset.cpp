/// \file   asset.cpp
///
/// \brief
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
#include <esl/economics/asset.hpp>
#include <esl/economics/accounting/standard.hpp>
#include <esl/simulation/identity.hpp>

namespace esl::economics {

    asset::asset(identity<property> i)
    : property(i)
    {

    }

    std::string asset::name() const
    {
        return "asset";
    }

    ///
    /// \param a    Valuation method and accounting rules
    /// \return
    //[[nodiscard]] price asset::value(const accounting::standard &a, const quantity &amount) const
    //{
    //    return price(0ll, a.reporting_currency);
    //}
}

/*
#ifdef WITH_PYTHON
#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(asset)
{
    class_<esl::economics::asset, bases<esl::law::property>>(
        "asset", init<esl::identity<esl::economics::asset>>())
        .def("name", &esl::economics::asset::name)
        .def("value", &esl::economics::asset::value)
        ;

}

#endif  // WITH_PYTHON
*/