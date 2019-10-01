/// \file   legal_entity.cpp
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
#include <esl/law/legal_entity.hpp>

#ifdef WITH_PYTHON
#include <boost/python.hpp>

using namespace boost::python;

std::string python_legal_entity_local(esl::law::legal_entity &e)
{
    std::stringstream stream_;
    stream_.write(e.local.data(), e.local.size());
    return stream_.str();
}

std::string python_legal_entity_code(esl::law::legal_entity &e)
{
    std::stringstream stream_;
    stream_.write(e.code.data(), e.code.size());
    return stream_.str();
}

std::string python_legal_entity_checksum(esl::law::legal_entity &e)
{
    std::stringstream stream_;
    auto t = e.checksum();
    stream_<< std::get<0>(t) << std::get<1>(t);
    return stream_.str();
}


BOOST_PYTHON_MODULE(legal_entity)
{
    class_<esl::law::legal_entity>("legal_entity", init<std::string>())
        .add_property("local", &python_legal_entity_local)
        .add_property("code", &python_legal_entity_code)
        .def("checksum", &python_legal_entity_checksum)
        ;
}

#endif  // WITH_PYTHON