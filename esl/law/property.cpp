/// \file   property.cpp
///
/// \brief  Things that can be owned
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

#include <esl/law/property.hpp>

#include <sstream>

#include <boost/serialization/export.hpp>


namespace esl::law {
    property::property(esl::identity<property> i) 
    : entity<property>(i)
    {
    
    }

    std::string property::name() const
    {
        std::stringstream stream_;
        stream_ << "property" << ' ' << identifier;
        return stream_.str();
    }
}  // namespace esl::law


BOOST_CLASS_TRACKING(esl::law::property, boost::serialization::track_always)

BOOST_CLASS_EXPORT(esl::entity<esl::law::property>)
BOOST_CLASS_EXPORT(esl::law::property)


#ifdef WITH_PYTHON
#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(property)
{
    class_<esl::law::property, bases<esl::entity<esl::law::property>>>(
        "property", init<esl::identity<esl::law::property>>())
        .def("name", &esl::law::property::name);
}

#endif  // WITH_PYTHON