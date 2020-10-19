/// \file   python_module.cpp
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
#include <esl/python_module_esl.hpp>


#ifdef WITH_PYTHON
///
/// https://docs.python.org/3.7/c-api/intro.html#include-files
///
#define PY_SSIZE_T_CLEAN


///// #include <Python.h>
///// #Include <boost/python/detail/wrap_python.hpp>

#include <esl/agent.hpp>
#include <esl/exception.hpp>
#include <esl/quantity.hpp>
#include <esl/version.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/python.hpp>
using namespace boost::python;

namespace esl {
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
        
        ////////////////////////////////////////////////////////////////////////
        class_< agent
              , bases< entity<agent>
                     //, interaction::communicator
                     //, data::producer
                     >>
            ( "agent", init<identity<agent>>());
        ////////////////////////////////////////////////////////////////////////
        

        class_<esl::exception>("exception", init<std::string>())
            .def("message", &esl::exception::what)
            ;

        register_exception_translator<esl::exception>(translate_exception);

        ////////////////////////////////////////////////////////////////////////
        class_<quantity>(
            "quantity",init<>())
            .def(init<std::uint64_t>())
            .def(self += self)
            .def(self + self)
            .def(self -= self)
            .def(self - self)
                //.def(self *= self)
            .def(self * self)
                //.def(self /= self)
                //.def(self / self)
                //.def(self %= self)
                //.def(self % self)

                // conversion to floating point type
            .def(float_(self))

            .def(self *= std::uint64_t())
            .def(self * std::uint64_t())
            .def(self / std::uint64_t())
            ;
        ////////////////////////////////////////////////////////////////////////
        
        def("version", version);

        ////////////////////////////////////////////////////////////////////////
    }

}

#endif