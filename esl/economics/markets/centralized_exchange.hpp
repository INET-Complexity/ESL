/// \file   centralized_exchange.hpp
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
#include <esl/economics/markets/order_book/basic_book.hpp>
#include <esl/economics/markets/order_book/order.hpp>
using namespace esl::economics::markets::order_book;

#include <esl/economics/markets/quote_message.hpp>


namespace esl::economics::markets {

    ///
    /// \brief  Sent by client to register with exchange.
    ///
    struct logon_message
    : public interaction::message< logon_message
                                 , interaction::library_message_code<0x00C00>()
                                 >
    {
        std::unordered_map<ticker, std::uint64_t> supply;

    };


    ///
    /// \brief  Sent by client to deregister from exchange.
    ///
    struct logout_message
            : public interaction::message< logout_message
    , interaction::library_message_code<0x00C01>()
    >
    {

    };





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
        /// \brief      How many limit levels of the order book to request >= 1. If the request type is `cancel`,
        ///             this is ignored, as all data will be canceled.
        ///
        std::uint16_t depth;

        ///
        /// \param instruments
        /// \param type
        /// \param depth
        explicit
        market_data_request_message( const std::vector<ticker> &instruments
                                   , request_t type = request_t::snapshot
                                   , std::uint16_t depth = 1
                                   )
        : instruments(instruments)
        , type(type)
        , depth(depth)
        {
            assert(depth > 0);
        }


    };


    ///
    /// \brief  Requests the status of an order, or if no order is specified, all orders
    ///
    struct order_status_request
    : public interaction::message< order_status_request
                         , interaction::library_message_code<0x00C03>()
                         >
    {

    };


    ///
    /// \brief  A structure with the status of one or more orders
    ///
    struct order_status_response_message
    : public markets::quote_message< order_status_response_message
                                   , interaction::library_message_code<0x00C04>()
                                   >
    {

    };


    ///
    /// \brief
    ///
    struct new_order_single
            : public markets::quote_message< new_order_single
    , interaction::library_message_code<0x00C05>()
    >
    {
        limit_order order_details;


    };


    ///
    /// \brief  Places multiple orders on the exchange, assuring that all orders are executed in sequence without
    ///         interleaving other market participants' orders as can happen with multiple single-order messages.
    ///
    struct new_order_list
            : public markets::quote_message< new_order_list
    , interaction::library_message_code<0x00C06>()
    >
    {
        std::vector<limit_order> order_details;


    };


    ///
    /// \brief
    ///
    struct cancel_order
            : public markets::quote_message< cancel_order
    , interaction::library_message_code<0x00C07>()
    >
    {


    };


    ///
    /// \brief
    ///
    struct replace_order
    : public markets::quote_message< replace_order
    , interaction::library_message_code<0x00C08>()
    >
    {


    };


    ///
    /// \brief
    ///
    struct execution_report_message
    : public interaction::message< execution_report_message
    , interaction::library_message_code<0x00C09>()
    >
    {

    };



    ///
    /// \brief  The sender asks the exchange to update them of their trading position
    ///
    struct request_for_position
    : public interaction::message< request_for_position
    , interaction::library_message_code<0x00C0A>()
    >
    {

    };



    ///
    /// \brief  A market that is organized as one or more order books.
    ///
    ///
    class centralized_exchange
    : public market
    {
    public:
        std::unordered_map<ticker, std::shared_ptr<order_book::basic_book>> books;

        ///
        /// \brief  For a session (period defined by logon and logout), this tracks the trading positions per
        ///         market participant.
        ///
        ///
        /// \details    Clearing constraint: supply + bought - sold > 0 for all agents, unless there are
        ///             offsetting positions or other rules that allow not settling in the underlier.
        ///             min(bought, sold) is the liquidity provision
        ///
        ///
        struct position_report
        {
            ///
            /// \brief  The number of tradeable economic property brought to the exchange by the agent
            ///
            std::uint64_t supply = 0;

            ///
            /// \brief
            ///
            std::uint64_t bought = 0;
            std::uint64_t sold   = 0;

            ///
            /// \brief  How much volume did the participant generate that did not affect their position?
            ///
            /// \return `min(bought, sold)`
            [[nodiscard]] constexpr std::uint64_t liquidity_provided() const
            {
                return std::min(bought, sold);
            }

            ///
            /// \brief  How much volume did the participant trade that directly affected their position?
            ///
            /// \return `abs(bought - sold)`
            [[nodiscard]] constexpr std::uint64_t liquidity_taken() const
            {
                if(bought >= sold){
                    return bought - sold;
                }
                return sold - bought;
            }
        };

        ///
        /// \brief  Maintains trading positions of all market participants.
        ///
        std::unordered_map<ticker, std::unordered_map<identity<agent>, position_report>> positions;


        ///
        /// \param m
        /// \param ti
        /// \param s
        /// \return
        simulation::time_point logon_participant( std::shared_ptr<logon_message> m
                                                , simulation::time_interval ti
                                                , std::seed_seq &s)
        {
            (void) s;
            for(const auto &t: traded_properties){
                std::unordered_map<identity<agent>, position_report> individual_positions_;
                position_report position_;

                if(m->supply.find(t) != m->supply.end()){
                    position_.supply = m->supply.find(t)->second;
                }
                individual_positions_.emplace(m->sender, position_);
                positions.emplace(t, individual_positions_);
            }
            return ti.upper;
        }


        ///
        /// \param m
        /// \param ti
        /// \param s
        /// \return
        simulation::time_point logout_participant( std::shared_ptr<logout_message> m
                                                  , simulation::time_interval ti
                                                  , std::seed_seq &s)
        {
            (void) s;
            for(const auto &t: traded_properties){
                // TODO: remove position, clear trades, warning if trades do not clear?
            }
            return ti.upper;
        }


        ///
        /// \brief  Create a market for a collection of tradeable properties.
        ///
        centralized_exchange( identity<agent> i
                            , std::vector<markets::ticker> traded
                            )
        : market(i, traded)
        {
            books.reserve(traded.size());

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////


            ESL_REGISTER_CALLBACK( logon_message
                                 , 0
                                 , [&](auto m
                                         , auto ti
                                         , auto &s){return this->logon_participant(m, ti, s);}
                                 , "logon new participant");

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ESL_REGISTER_CALLBACK( logout_message
                                   , 0
                                   , [&](auto m
                                           , auto ti
                                           , auto &s){return this->logout_participant(m, ti, s);}
                                   , "logout participant");


            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ESL_REGISTER_CALLBACK( market_data_request_message
                                   , 0
                                   , [&]( std::shared_ptr<market_data_request_message> m
                                           , simulation::time_interval ti
                                           , std::seed_seq &s){

                return ti.upper;
                }, "logout participant");



        }

        virtual ~centralized_exchange() = default;

        ///
        ///
        ///
        /// \param order
        void insert(const limit_order &order)
        {
            auto i = books.find(order.symbol);

            if(i == books.end()) {
                //this->template create_message<>();
            }
            return i->second->insert(order);
        }

        ///
        /// \param symbol
        /// \param identifier
        void cancel(const ticker& symbol, const basic_book::order_identifier identifier)
        {
            auto i = books.find(symbol);
            if(books.end() != i){
                i->second->cancel(identifier);
            }
        }


        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(market);
        }

    };

};

#endif //ME_EXCHANGE_HPP
