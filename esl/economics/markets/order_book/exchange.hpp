/// \file   exchange.hpp
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
#ifndef ME_EXCHANGE_HPP
#define ME_EXCHANGE_HPP


#include <esl/economics/markets/market.hpp>
#include <esl/economics/markets/order_book/book.hpp>


namespace esl::economics::markets::order_book {

    class exchange
    : public esl::economics::market
    {
    private:

    public:
        using market::market;

        ///
        /// \brief  Create a market for a set of assets.
        exchange(identity<agent> i, law::property_map<quote> traded = {});

        virtual ~exchange() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(market);
        }

    };

};

#endif //ME_EXCHANGE_HPP
