/// \file   python_module.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-08-30
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
#ifndef ESL_SIMULATION_PYTHON_MODULE_HPP
#define ESL_SIMULATION_PYTHON_MODULE_HPP


#ifdef WITH_PYTHON
#include <esl/simulation/entity.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/converter/shared_ptr_to_python.hpp>

namespace esl::simulation::python_module {
    ///
    /// \brief
    ///
    template<typename parent_t_>
    void helper_create_child_identity(parent_t_ &e)
    {
       // e.create<>()
    }

    ///
    /// \brief
    ///
    void helper_create_child_entity(const boost::python::object& python_object)
    {
        boost::python::extract<boost::python::object> extractor_(python_object);
        boost::python::object object_ = extractor_();
        auto name_ = boost::python::extract<std::string>(
            object_.attr("__class__").attr("__name__"));

        //return boost::python::converter::shared_ptr_to_python(
        //    std::make_shared()
        //);
    }




} // namespace esl::simulation::python_module

#endif // WITH_PYTHON

#endif  // ESL_SIMULATION_PYTHON_MODULE_HPP
