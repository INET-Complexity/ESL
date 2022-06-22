/// \file   market_data_request_message.hpp
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
#ifndef ME_MARKET_DATA_REQUEST_MESSAGE_HPP
#define ME_MARKET_DATA_REQUEST_MESSAGE_HPP

#include <esl/interaction/message.hpp>


namespace esl::economics::markets {
    ///
    /// \brief  The market participant (sender) specifies what kind of
    ///         market data feed is required (none, best bid and offer stream, all order stream)
    ///
    /// \details    FIX 4.4 : Market Data Request <V> message
    ///
    struct market_data_request_message
    : public interaction::message< market_data_request_message
                                 , interaction::library_message_code<0x00C02>()
                                 >
    {
        ///
        ///
        /// \details    instruments.size() = NoRelatedSym
        std::vector<ticker> instruments;

        ///
        ///
        /// \details    FIX 4.4 : SubscriptionRequestType <263>
        enum request_t
        { snapshot = 0
        , stream   = 1
        , cancel   = 2
        } type;

        ///
        /// \brief      How many limit levels of the order book to request. If zero, only receive trades
        /// \details    If the request type is `cancel`,
        ///             this is ignored, as all data will be canceled.
        ///
        std::uint8_t depth;

        ///
        /// \param instruments
        /// \param type
        /// \param depth
        explicit
        market_data_request_message( const identity<agent> &sender     = identity<agent>()
                                   , const identity<agent> &recipient  = identity<agent>()
                                   , simulation::time_point sent       = simulation::time_point()
                                   , simulation::time_point received   = simulation::time_point()
                                   , const std::vector<ticker> &instruments = {}
                                   , request_t type = request_t::snapshot
                                   , std::uint8_t depth = 0
                                   )
        : interaction::message< market_data_request_message
                               , interaction::library_message_code<0x00C02>()
                               >(sender, recipient, sent, received)
        , instruments(instruments)
        , type(type)
        , depth(depth)
        {

        }


        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
 
            archive &boost::serialization::make_nvp( "message⟨market_data_request_message⟩"
                                                   , boost::serialization::base_object<
                                                            interaction::message<market_data_request_message,interaction::library_message_code<0x00C02>()>
                                                                                      >(*this));
            archive &BOOST_SERIALIZATION_NVP(instruments);

            archive &BOOST_SERIALIZATION_NVP(type);

            archive &BOOST_SERIALIZATION_NVP(depth);
        }
    };
}//namespace esl::economics::markets


#endif//ME_MARKET_DATA_REQUEST_MESSAGE_HPP