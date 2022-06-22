/// \file   position_request_message.hpp
///
/// \brief
///
/// \authors    Maarten
/// \date       2020-06-24
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
#ifndef ESL_POSITION_REQUEST_MESSAGE_HPP
#define ESL_POSITION_REQUEST_MESSAGE_HPP

#include <esl/interaction/message.hpp>
#include <esl/economics/markets/order_book/order.hpp>

namespace esl::economics::markets {
    ///
    /// \brief  The sender asks the exchange to update them of their trading
    ///         position.
    ///
    struct position_request_message
    : public interaction::message< position_request_message
                                 , interaction::library_message_code<0x00C0A>()
                                 >
    {
        // TODO
    };
}//namespace esl::economics::markets


#endif//ESL_POSITION_REQUEST_MESSAGE_HPP