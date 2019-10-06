/// \file   transfer.hpp
///
/// \brief  A transfer message moves one or properties from one agent to
///         another. The message is specialised for fungible and infungible
///         property types, as it will apply the appropriate datastructure
///         to hold these properties.
///
/// \authors    Maarten P. Scholl
/// \date       2018-06-04
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
#ifndef ESL_TRANSFER_HPP
#define ESL_TRANSFER_HPP

#include <esl/economics/accounting/inventory.hpp>
#include <esl/interaction/header.hpp>
#include <esl/interaction/message.hpp>
#include <esl/simulation/identity.hpp>

namespace esl::law {
    template<typename property_t_>
    struct owner;
}

namespace esl::interaction {
    ///
    /// \brief  A transfer between two property holders moves one or more
    ///         properties from one agent (the transferor) to the
    ///         other (the transferee). The transfer is also a message, and the
    ///         agent sending the message is not necessarily transferor. For
    ///         example, in cases where only the receiving party in an
    ///         obligation contract can determine when to initiate the transfer
    ///         (such as a callable derivatives instrument), the message
    ///         recipient is the transferor.
    ///
    struct transfer
    : public esl::interaction::message<transfer, library_message_code<0x2>()>
    {
        ///
        /// \brief  The transferor sends the property
        ///
        identity<esl::law::owner<law::property>> transferor;

        ///
        /// \brief The transferee receives the property
        ///
        identity<esl::law::owner<law::property>> transferee;

        ///
        /// \brief  The property that is being transferred
        ///
        economics::accounting::inventory_filter<law::property> transferred;

        ///
        /// \brief
        ///
        /// \param transferor
        /// \param transferee
        /// \param transferred
        transfer(
            const identity<agent> &sender    = identity<agent>(),
            const identity<agent> &recipient = identity<agent>(),
            const identity<law::owner<law::property>> &transferor =
                identity<law::owner<law::property>>(),
            const identity<law::owner<law::property>> &transferee =
                identity<law::owner<law::property>>(),
            const economics::accounting::inventory_filter<law::property>
                &transferred =
                    economics::accounting::inventory_filter<law::property>())
        : esl::interaction::message<transfer, library_message_code<0x2>()>(
            sender,
            recipient)
        , transferor(transferor)
        , transferee(transferee)
        , transferred(transferred)
        {}

        virtual ~transfer() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            typedef esl::interaction::message<transfer,
                                              library_message_code<0x2>()>
                message_0x2;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(message_0x2);
            archive &BOOST_SERIALIZATION_NVP(transferor);
            archive &BOOST_SERIALIZATION_NVP(transferee);
            archive &BOOST_SERIALIZATION_NVP(transferred);
        }
    };
}  // namespace esl::interaction

#endif  // ESL_TRANSFER_HPP
