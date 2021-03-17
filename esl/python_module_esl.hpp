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
#ifndef ESL_PYTHON_MODULE_ESL_HPP
#define ESL_PYTHON_MODULE_ESL_HPP


#ifdef WITH_PYTHON
///
/// https://docs.python.org/3.7/c-api/intro.html#include-files
///
#define PY_SSIZE_T_CLEAN

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>

#include <esl/agent.hpp>

#include <esl/simulation/python_module_simulation.hpp>

namespace esl::python_module {
    template<typename agent_t_, typename ... arguments_>
    static boost::shared_ptr<agent_t_> python_construct_agent( boost::python::object const &o, arguments_ ... a )
    {
        boost::python::extract<esl::simulation::python_module::python_identity> e(o);

        if(!e.check()){

        }

        esl::simulation::python_module::python_identity pi = e();

        //identity<agent_t_> i = esl::reinterpret_identity_cast<identity<agent_t_>>(pi);

        return boost::make_shared<agent_t_>(identity<agent_t_>(pi.digits), a...);
    }


}

#endif


#endif  // ESL_PYTHON_MODULE_ESL_HPP
