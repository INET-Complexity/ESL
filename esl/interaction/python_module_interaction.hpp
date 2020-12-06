/// \file   python_module_interaction.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-11-27
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
#ifndef ESL_PYTHON_MODULE_INTERACTION_HPP
#define ESL_PYTHON_MODULE_INTERACTION_HPP


#ifdef WITH_PYTHON
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
using namespace boost::python;

#include <esl/interaction/communicator.hpp>
#include <esl/interaction/header.hpp>
#include <esl/interaction/message.hpp>

namespace esl::interaction {

    ///
    /// \brief  Takes a python function object and wraps it, so we can call it
    ///         from C++
    ///
    /// \details    See communicator.hpp for callback_handle type definition
    ///
    ///
    /// \param f The python function that handles the message
    /// \return
    communicator::callback_handle make_callback_handle(boost::python::object f)
    {
        //  std::function<simulation::time_point( message_t
        //                                      , simulation::time_interval
        //                                      , std::seed_seq &
        //                                      )>;

        return [&]( communicator::message_t m
                  , simulation::time_interval i
                  , std::seed_seq &s
                  )
        {
            auto result_ = f(m, i, s);
            return boost::python::extract<simulation::time_point>(result_);
        };
    }

    ///
    /// \brief  Since the main class is a template, we must expose a
    ///         non-template to python.
    ///
    class python_message
    : public message<python_message, library_message_code<0x1u>()>
    {
    public:
        // this helps the linker resolve the message code
        constexpr const static message_code python_code = code;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            //BOOST_SERIALIZATION_BASE_OBJECT_NVP(message);
            archive &boost::serialization::make_nvp("message",
                boost::serialization::base_object<message<python_message, library_message_code<0x1u>()>>(
                    *this));

        }
    };
}

#endif  // WITH_PYTHON


#endif  // ESL_PYTHON_MODULE_INTERACTION_HPP
