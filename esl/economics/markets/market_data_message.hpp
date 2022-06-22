/// \file   market_data_message.hpp
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
#ifndef ESL_MARKET_DATA_MESSAGE_HPP
#define ESL_MARKET_DATA_MESSAGE_HPP

#include <esl/interaction/message.hpp>
#include <esl/economics/markets/order_book/order.hpp>

namespace esl::economics::markets {
    ///
    /// \brief
    ///
    struct market_data_message
    : public interaction::message<market_data_message, interaction::library_message_code<0x00C0B>()>
    {
        /// 
        /// \brief  
        /// 
        time_interval period;

        ticker symbol;

        // TODO: decide to add timestamp or not 

        std::vector<std::pair<quote, size_t>> ask;

        std::vector<std::pair<quote, size_t>> bid;
        
        std::vector<std::pair<quote, size_t>> trades;

        explicit market_data_message( const identity<agent> &sender    = identity<agent>()
                                    , const identity<agent> &recipient = identity<agent>()
                                    , simulation::time_point sent      = simulation::time_point()
                                    , simulation::time_point received  = simulation::time_point()
                                    )
        : interaction::message< market_data_message
                              , interaction::library_message_code<0x00C0B>()
                              >::message( sender
                                        , recipient
                                        , sent
                                        , received
                                        )
        {

        }
    };
}//namespace esl::economics::markets


#endif//ESL_MARKET_DATA_MESSAGE_HPP