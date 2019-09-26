/// \file   exception.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-24
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
#ifdef WITH_PYTHON
///
/// https://docs.python.org/3.7/c-api/intro.html#include-files
///
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif

#include <esl/exception.hpp>


namespace esl {
    exception::exception(const std::string &message)
    : message_(message)
    {

    }

    const char *exception::what() const noexcept
    {
        return message_.c_str();
    }
}


#ifdef WITH_PYTHON
#include <boost/python.hpp>

    ///
    /// \brief
    ///
    /// \param e
    void translate_exception(const esl::exception &e)
    {
        PyErr_SetString(PyExc_UserWarning, e.what());
    }


    using namespace boost::python;
    BOOST_PYTHON_MODULE(exception)
    {
        class_<esl::exception>("exception", init<std::string>())
        .def("message", &esl::exception::what)
        ;

        register_exception_translator<esl::exception>(translate_exception);
    }

#endif
