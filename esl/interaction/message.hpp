/// \file   message.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-02
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
/// Oxford Martin School, University of Oxford
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
#ifndef ESL_MESSAGE_HPP
#define ESL_MESSAGE_HPP

#include <esl/computation/type_code.hpp>
#include <esl/computation/type_marker.hpp>
#include <esl/interaction/header.hpp>


namespace esl::interaction {

    template<typename message_type_, message_code type_code_>
    class message
    : public header
    , public type_code<type_code_>
    , public type_marker<message_type_>
    {
    public:
        explicit message(
            identity<agent> sender          = identity<agent>(),
            identity<agent> recipient       = identity<agent>(),
            simulation::time_point sent     = simulation::time_point(),
            simulation::time_point received = simulation::time_point())
        : header(type_code_, sender, recipient, sent, received)
        {}

        virtual ~message() = default;

        ///
        /// \warning    any error with description along the lines of `error:
        ///             ambiguating new declaration` indicates that
        ///             two message types were mapped to the same integer type
        ///             code
        /// @return
        ///
        friend esl::type_marker<message_type_> code(esl::type_code<type_code_>)
        {
            return esl::type_marker<message_type_>();
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;

            BOOST_SERIALIZATION_BASE_OBJECT_NVP(header);

            archive &boost::serialization::make_nvp(
                "type_code_t⟨type_code_⟩",
                boost::serialization::base_object<type_code<type_code_>>(
                    *this));

            archive &boost::serialization::make_nvp(
                "market_type⟨message_type_⟩",
                boost::serialization::base_object<type_marker<message_type_>>(
                    *this));
        }
    };


#ifdef WITH_PYTHON
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

            archive &boost::serialization::make_nvp(
                "message",
                boost::serialization::base_object<message<python_message, library_message_code<0x1u>()>>(
                    *this));

        }
    };
#endif  // WITH_PYTHON

}  // namespace esl::interaction

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<typename message_type_, uint64_t type_code_>
    struct is_mpi_datatype<esl::interaction::message<message_type_, type_code_>>
    : public mpl::false_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_MESSAGE_HPP
