/// \file   header.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-02
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
#ifndef ESL_HEADER_HPP
#define ESL_HEADER_HPP

#include <utility>
#include <cstdint>
#include <type_traits>

#include <esl/simulation/identity.hpp>
#include <esl/simulation/time.hpp>

namespace esl {

    class agent;

    namespace interaction {

        typedef std::uint64_t message_code;
        ///
        /// \brief
        ///
        constexpr message_code message_mask = 0x8000'0000'0000'0000;

        ///
        /// \tparam message_offset_
        /// \return
        template<message_code offset_>
        constexpr message_code library_message_code()
        {
            static_assert(message_mask >= offset_,
                          "maximum code offset exceeded");
            return message_mask | offset_;
        }

        ///
        /// \tparam message_code_
        /// \return
        template<message_code code_>
        constexpr message_code library_message_offset()
        {
            static_assert(message_mask & code_, "not a library message code");
            return (~message_mask) & code_;
        }


        ///
        /// \brief  The basic components in a message that facilitate delivery
        ///
        struct header
        {
            ///
            /// \brief
            ///
            const message_code type;

            ///
            ///
            ///
            identity<agent> sender;

            ///
            /// \brief
            ///
            identity<agent> recipient;

            ///
            /// The time point at which the message was sent by the sender.
            ///
            simulation::time_point sent;

            ///
            /// The earliest time point at which the message may be received as
            /// determined by the sender. This point may lie far in the future.
            ///
            simulation::time_point received;

            ///
            /// \brief
            ///
            /// \param type
            explicit header(message_code type = 0)
            : type(type), sender(), recipient(), sent(), received()
            {}

            ///
            ///
            header(const header &) = default;

            ///
            ///
            ///
            header(header &&) = default;

            ///
            /// \param h
            /// \return
            header &operator=(const header &h)
            {
                const_cast<message_code &>(type) = h.type;
                sender                           = h.sender;
                recipient                        = h.recipient;
                sent                             = h.sent;
                received                         = h.received;
                return *this;
            }

            ///
            /// \param type
            /// \param sender
            /// \param recipient
            /// \param sent
            /// \param received
            header(uint64_t type, identity<agent> sender,
                   identity<agent> recipient,
                   simulation::time_point sent     = simulation::time_point(),
                   simulation::time_point received = simulation::time_point())
            : type(type)
            , sender(std::move(sender))
            , recipient(std::move(recipient))
            , sent(sent)
            , received(received)
            {}

            ///
            ///
            virtual ~header() = default;

            ///
            /// \tparam archive_t
            /// \param archive
            /// \param version
            template<class archive_t>
            void serialize(archive_t &archive, const unsigned int version)
            {
                (void)version;
                archive &boost::serialization::make_nvp(
                    "type", const_cast<message_code &>(type));

                archive &BOOST_SERIALIZATION_NVP(sender);
                archive &BOOST_SERIALIZATION_NVP(recipient);
                archive &BOOST_SERIALIZATION_NVP(sent);
                archive &BOOST_SERIALIZATION_NVP(received);
            }
        };
    }  // namespace interaction
}  // namespace esl

// this is imported for the type computation below.
#include <esl/simulation/identity.hpp>


#ifdef WITH_MPI
#include <boost/mpi.hpp>

namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::interaction::header>
    : public mpl::and_< is_mpi_datatype<esl::interaction::message_code>::type
                      , is_mpi_datatype<esl::identity<esl::agent>>::type
                      , is_mpi_datatype<esl::simulation::time_point>::type
                      >::type
    {

    };
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_HEADER_HPP
