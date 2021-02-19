/// \file   order.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-11-19
/// \copyright  Copyright 2017-2021 The Institute for New Economic Thinking,
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
#ifndef MARKET_ECOLOGY_IMPACT_FUNCTION_ORDER_HPP
#define MARKET_ECOLOGY_IMPACT_FUNCTION_ORDER_HPP


#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>

#include <esl/economics/markets/order_message.hpp>


namespace esl::economics::markets::impact_function {

    struct order_message
    : public esl::economics::markets::order_message<order_message, interaction::library_message_code<0x00B2U>()>
    {

        enum side_t
        { buy
        , sell
        } side;

        law::property_map<std::pair<std::uint64_t, side_t>> volumes;


        order_message( identity<agent> sender
                      , identity<agent> recipient
                      , simulation::time_point sent     = simulation::time_point()
                      , simulation::time_point received = simulation::time_point())
        : markets::order_message<order_message, interaction::library_message_code<0x00B2U>()>
            (sender, recipient, sent, received)
        {

        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            typedef markets::order_message<order_message, interaction::library_message_code<0x00B2U>()>
                order_message_type;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(order_message_type);
        }
    };
}

#endif  // MARKET_ECOLOGY_IMPACT_FUNCTION_ORDER_HPP
