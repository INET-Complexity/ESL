/// \file   output_base.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-26
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
#include <esl/data/output_base.hpp>

namespace esl::data {

    output_base::output_base(const std::string &name)
    : name(name)
    {

    }

}




#ifdef WITH_PYTHON
#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(output_base)
{
    class_<esl::data::output_base>("output_base", init<std::string>())
    .def_readonly("name", &esl::data::output_base::name)
    ;
}

#endif  // WITH_PYTHON