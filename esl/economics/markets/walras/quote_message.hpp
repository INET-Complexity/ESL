/// \file   quote_message.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-25
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
#ifndef ESL_WALRAS_QUOTE_MESSAGE_HPP
#define ESL_WALRAS_QUOTE_MESSAGE_HPP


#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>

#include <esl/economics/markets/quote_message.hpp>


namespace esl::economics::markets::walras {
    struct quote_message
    : public markets::quote_message<
          quote_message, esl::interaction::library_message_code<0x00A0U>()>
    {
        explicit quote_message(
            identity<agent> sender            = identity<agent>(),
            identity<agent> recipient         = identity<agent>(),
            law::property_map<quote> proposed = {},
            simulation::time_point sent       = simulation::time_point(),
            simulation::time_point received   = simulation::time_point())
        : markets::quote_message<
            quote_message, esl::interaction::library_message_code<0x00A0U>()>(
            std::move(sender), std::move(recipient), proposed,
            markets::indication::indicative, sent, received)
        {}

        virtual ~quote_message() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            typedef markets::quote_message<
                quote_message,
                esl::interaction::library_message_code<0x00A0U>()>
                quote_message_base_type;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(
                quote_message_base_type);
        }
    };
}  // namespace esl::economics::walras

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {

    template<>
    struct is_mpi_datatype<esl::economics::markets::walras::quote_message>
    : public mpl::false_
    {

    };

}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_WALRAS_QUOTE_MESSAGE_HPP
