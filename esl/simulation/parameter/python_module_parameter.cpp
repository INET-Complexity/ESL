/// \file   python_module_parameter.cpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-11-18
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
#include <esl/simulation/parameter/parametrization.hpp>

#ifdef WITH_PYTHON

#include <vector>
using std::vector;
using std::dynamic_pointer_cast;
#include <optional>

#include <boost/make_shared.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
using namespace boost::python;

#include <esl/exception.hpp>



namespace esl::simulation::parameter {

    template<typename parameter_t_>
    std::optional<boost::python::object> pack(std::shared_ptr<parameter_base> parameter)
    {
        auto value_ =
            dynamic_pointer_cast<constant<parameter_t_>>(parameter);

        if(value_) {
            return boost::python::object(parameter_t_(*value_));
        }
        return {};
    }

    boost::python::object get_helper(parametrization &p, const std::string &name)
    {
        auto parameter_ = p.values.find(name);

        if(p.values.end() == parameter_) {
            throw esl::exception("parametrization[" + name + "]");
        }

        auto result_ = pack<double>(parameter_->second);
        if(result_){
            return result_.value();
        }

        result_ = pack<std::int64_t>(parameter_->second);
        if(result_){
            return result_.value();
        }


        result_ = pack<std::uint64_t>(parameter_->second);
        if(result_){
            return result_.value();
        }

        throw esl::exception("parametrization[" + name + "]");
    }



    BOOST_PYTHON_MODULE(_parameter)
    {
        class_<parameter_base>("parameter_base", init<>());

        class_<constant<double>>("constant_double", init<double>());
        class_<constant<std::int64_t>>("constant_int64", init<int64_t>());
        class_<constant<std::uint64_t>>("constant_uint64", init<uint64_t>());

        class_<parametrization>("parametrization", init<>())
            .def("get", get_helper)

            ;
    }
}

#endif