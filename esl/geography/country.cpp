/// \file   country.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-27
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

#include <esl/geography/country.hpp>


#ifdef WITH_PYTHON
#include <boost/python.hpp>

#include <string>

std::string python_country_code(const esl::geography::country &c)
{
    return (std::string() + c.code[0]) + c.code[1];
}


using namespace boost::python;
BOOST_PYTHON_MODULE(country)
{
    class_<esl::geography::country>("country")
        .add_property("code", python_country_code)
        ;
}
#endif
