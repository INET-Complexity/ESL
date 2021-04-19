/// \file   python_module.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
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
#include <esl/python_module_esl.hpp>


#ifdef WITH_PYTHON

#include <esl/agent.hpp>
#include <esl/exception.hpp>
#include <esl/version.hpp>
#include <esl/quantity.hpp>
#include <esl/simulation/python_module_simulation.hpp>


#include <esl/simulation/model.hpp>

#include <esl/computation/block_pool.hpp>
#include <esl/computation/environment.hpp>
#include <esl/computation/timing.hpp>
using esl::computation::agent_timing;



using namespace boost::python;


namespace esl {
//    static boost::shared_ptr<agent> python_construct_agent( object const &o )
//    {
//        boost::python::extract<esl::simulation::python_module::python_identity> e(o);
//        return boost::make_shared<agent>(identity<agent>(e().digits));
//    }

    ///
    /// \brief  Translates C++ exceptions to Python errors, by setting the
    ///         Python error text equal to the C++ exception message.
    ///
    /// \param e
    void translate_exception(const exception &e)
    {
        PyErr_SetString (   PyExc_RuntimeError
                            //PyExc_UserWarning
                        , e.what()
                        );
    }

    ///
    /// \brief  Python module structure follows directory structure and
    ///         C++ namespaces. We use __init__.py for submodules, so we can mix
    ///         native code and Python-binding specific Python code. Thus, we
    ///         protect the name of the module with an underscore, and import it
    ///         from __init__.py
    ///
    BOOST_PYTHON_MODULE(_esl)
    {
        class_<esl::exception>("exception", init<std::string>())
            .def("message", &esl::exception::what)
            ;

        register_exception_translator<esl::exception>(translate_exception);

        class_<quantity>(
            "quantity",init<>())
            .def(init<std::uint64_t>())
            .def(self += self)
            .def(self + self)
            .def(self -= self)
            .def(self - self)
            //.def(self *= self)
            .def(self * self)

            .def(self < self)
            .def(self > self)
            .def(self == self)
            .def(self != self)
            .def(self <= self)
            .def(self >= self)
                //.def(self /= self)
                //.def(self / self)
                //.def(self %= self)
                //.def(self % self)
                // conversion to floating point type
            .def(float_(self))
            .def("__repr__", &quantity::representation)
            .def("__str__", &quantity::representation)
            .def(self *= std::uint64_t())
            .def(self * std::uint64_t())
            .def(self / std::uint64_t())
            ;
//
//        class_<agent>("agent")
//            .def("__init__", boost::python::make_constructor(&python_construct_agent))
//            ;

        def("version", version, "Print the library's version.");

        ////////////////////////////////////////////////////////////////////////

        {
            std::string nested_name = boost::python::extract<std::string>(boost::python::scope().attr("__name__") + ".computation");
            boost::python::object nested_module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule(nested_name.c_str()))));
            boost::python::scope().attr("computation") = nested_module;
            boost::python::scope parent = nested_module;

            class_<computation::block_pool::block<object>>("block"
                                                          , "The block is a unit of memory used by the block_pool allocator, and has fixed size.")
                .def_readwrite("data", &computation::block_pool::block<object>::data)
                .def_readwrite("index", &computation::block_pool::block<object>::index)
                ;

            // timing information
            class_<computation::agent_timing>("agent_timing", "Contains performance metrics for individual agents.")
                .def_readwrite("messaging", &agent_timing::messaging)
                .def_readwrite("acting", &agent_timing::acting);

        }


        {
            std::string nested_name = boost::python::extract<std::string>(boost::python::scope().attr("__name__") + ".ns2");
            boost::python::object nested_module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule(nested_name.c_str()))));
            boost::python::scope().attr("ns2") = nested_module;
            boost::python::scope parent = nested_module;

            def("version2", version, "Print the library's version too.");

        }


    }

}

#endif