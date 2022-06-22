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
using esl::simulation::time_interval;

#include <esl/interaction/communication.hpp>
using esl::interaction::communication::latency_model;


#include <esl/economics/markets/logon_message.hpp>
#include <esl/economics/markets/logout_message.hpp>
#include <esl/economics/markets/market_data_request_message.hpp>
#include <esl/economics/markets/order_status_request_message.hpp>
#include <esl/economics/markets/new_order_single_message.hpp>
#include <esl/economics/markets/new_order_list_message.hpp>
#include <esl/economics/markets/cancel_order_message.hpp>
#include <esl/economics/markets/replace_order_message.hpp>
#include <esl/economics/markets/execution_report_message.hpp>
#include <esl/economics/markets/position_request_message.hpp>
#include <esl/economics/markets/market_data_message.hpp>


namespace esl::economics::markets {
    ///
    /// \brief  A market that is organized as one or more order books.
    ///
    ///
    class centralized_exchange
    : public market
    , public identifiable_as<centralized_exchange>
    {
    public:
        ///
        /// \brief
        ///
        boost::container::flat_map<ticker, std::shared_ptr<order_book::basic_book>> books;

        ///
        /// \brief  When the market is open?
        ///  TODO: generalize to include multiple market modes, e.g. post session auctions
        ///

        struct session
        {
            time_interval open;
            
            enum class session_t 
            { continuous
            , batch_auction
            } type;

            std::vector<ticker> traded;
        };

        /// 
        /// \brief The sessions that the exchange will go through in the simulation
        /// 
        std::vector<session> sessions;

        /// 
        /// \brief  
        /// 
        std::shared_ptr<latency_model> communication_model;

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
            /// \brief  The number of tradable economic property brought to the exchange by the agent
            ///
            std::uint64_t supply;

            ///
            /// \brief  The number of tradable property bought during the session
            ///
            std::uint64_t bought;

            ///
            /// \brief  The number of tradable property sold during the session
            ///
            std::uint64_t sold;

            ///
            /// \param supply
            /// \param bought
            /// \param sold
            constexpr explicit position_report( std::uint64_t supply = 0
                                              , std::uint64_t bought = 0
                                              , std::uint64_t sold = 0
                                              )
            : supply(supply)
            , bought(bought)
            , sold(sold)
            {

            }

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

            friend std::ostream &operator<<(std::ostream &stream,
                                            const position_report &report)
            { 
                stream << report.supply << "+" << report.bought << "-"
                       << report.sold
                       << std::endl;
                return stream;
            }
        };

        ///
        /// \brief  Maintains trading positions of all market participants.
        ///
        boost::container::flat_map<ticker, boost::container::flat_map<identity<agent>, position_report>> positions;

        struct data_subscription
        {
            /// 
            /// 
            /// 
            std::uint8_t depth = 0; 

            ///
            /// \brief  time point from which point onwards to receive market info
            ///
            esl::simulation::time_point start = 0;

            /// 
            /// \brief  0 means fastest/all
            /// 
            esl::simulation::time_duration periodicity = 0;
        };

        boost::container::flat_map<ticker, boost::container::flat_map<identity<agent>, data_subscription> > market_data_subscriptions;

        ///
        /// \brief  contains the anonymised market data sent to data subscribers/requesters
        /// TODO: make this faster
        ///
        struct market_data
        {
            std::vector<std::pair<quote, size_t>> ask;
            std::vector<std::pair<quote, size_t>> bid;
            std::vector<std::pair<quote, size_t>> trades;
        };

        std::unique_ptr<esl::data::output< std::tuple< ticker
                                                     , quote
                                                     , identity<agent>
                                                     , limit_order::side_t
                                                     , decltype(limit_order::quantity)
                                                     , typename execution_report::state_t    
                                                     >
                                         >> output_orders;

        ///
        /// \brief  
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
                boost::container::flat_map<identity<agent>, position_report> individual_positions_;
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
                auto positions_ = positions.find(t);
                // nothing was traded in this market, so no positions recorded
                if(positions.end() == positions_) { 
                    continue;
                }

                auto individual_ = positions_->second.find(m->sender);
                // the individual did not trade in this market
                if(positions_->second.end() == individual_) {  
                    continue;
                }

                // test whether trades can clear 
                if(individual_->second.supply + individual_->second.bought < individual_->second.sold) {
                    LOG(warning)
                        << "participant " << m->sender
                        << " sold more than they supplied - trades won't clear"
                        << std::endl;
                    
                }
                positions_->second.erase(individual_);
                if(positions_->second.empty()) {
                    positions.erase(positions_);
                }
            }
            return ti.upper;
        }

        ///
        /// \brief  Handles a single order, and sends the reports that it generates.
        ///
        /// \param m
        /// \param ti
        /// \param s
        /// \return
        simulation::time_point insert_order( std::shared_ptr<new_order_single_message> m
                                           , simulation::time_interval ti
                                           , std::seed_seq &seed
                                           )
        {
            auto i = books.find(m->order_details.symbol);

            if(i == books.end()) {
                // In the real world, this would trigger some kind of message to
                // the trader saying the order is rejected. Here in the
                // simulation, we need to warn the modeller that an agent is
                // placing orders that can't be matched to any active market.
                throw esl::exception("Invalid ticker in limit order.");
            }

            order_book::basic_book &b = *(i->second);

            const auto ask_ = b.ask();
            const auto bid_ = b.bid();

            b.insert(m->order_details);

            market_data data_;

            for(const auto & r: b.reports){
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                //if(r.owner == m->sender) {
                //    if(r.state == execution_report::state_t::match) {
                //        auto state_ = "match  ";
                //        std::cout
                //            << state_ << r.owner << " " << r.identifier << " "
                //            << (r.side == limit_order::buy ? "buy " : "sell")
                //            << " " << r.limit << " " << r.quantity << std::endl;
                //    }
                //}
                //if(r.owner == m->sender) {
                //    std::string state_ = "insert ";
                //    if(r.state == execution_report::state_t::match) {
                //        state_         = "match  ";
                //    } else if(r.state == execution_report::state_t::cancel) {
                //        std::cout << "--------------------------------------------------------------------------------"
                //                  << std::endl;
                //        state_         = "cancel ";
                //    } else {
                //        std::cout << "--------------------------------------------------------------------------------"
                //                  << std::endl;
                //    }
                //    std::cout
                //        << state_ << r.owner << " " << r.identifier
                //        << " " << (r.side == limit_order::buy ? "buy " : "sell")
                //        << " " << r.limit << " " << r.quantity << std::endl;
                //    
                //    std::cout << std::endl;
                //    std::cout << b.orders() << std::endl;
                //    if(r.state == execution_report::state_t::cancel) {
                //        b.display(5);
                //    } else if(r.state == execution_report::state_t::placement) {
                //        b.display(5);
                //    }
                //}
                //std::cout << std::endl;
                //////////////////////////////////////////////////////////////////////////////////////////////////////


                // we do not log rejected messages, so that later analyses do not accidently use these
                // invalid orders should not be part of the simulation
                if(output_orders && r.state != execution_report::state_t::invalid && r.owner == m->sender) {
                    output_orders->put( ti.lower
                                      , std::make_tuple
                                        ( m->order_details.symbol
                                        , r.limit
                                        , r.owner
                                        , r.side
                                        , r.quantity
                                        , r.state
                                        ));
                }

                // notify the participants involved in the trade
                {
                    simulation::time_point arrival_ = ti.lower;
                    if(communication_model) {
                        arrival_ += communication_model->sample(identifier, m->sender, seed);
                    }
                    auto response_ = this->template create_message<execution_report_message>(r.owner, arrival_);
                    response_->sender = (*this);
                    response_->order  = m->order_details;
                    response_->sent   = ti.lower;
                    response_->report = r;
                }
                // if no one wants market data on this symbol, skip the next steps.
                if(market_data_subscriptions.end() == market_data_subscriptions.find(m->order_details.symbol)) {
                    continue;
                }
                // if the order resulted in a match, update market data feed (once, as there will be two reports)
                auto trade_ = (r.state == execution_report::state_t::match);
                // or if the bed bid or ask improved
                auto best_bid_ask_ = (ask_ != b.ask() || bid_ != b.bid());
                if((trade_ && r.owner == m->sender) || best_bid_ask_){
                    for(auto &[subscriber_, details_]: market_data_subscriptions[m->order_details.symbol]) {

                        if(details_.start + details_.periodicity > ti.lower) {
                            continue;
                        }

                        // change the start of the next data subscription fulfillment, so that we dont overload them
                        // 
                        details_.start = ti.lower;


                        simulation::time_point arrival_ = ti.lower;
                        if(communication_model) {

                            arrival_ += communication_model->sample(
                                identifier, subscriber_, seed);
                        }
                        auto market_data_ = this->template create_message<market_data_message>(subscriber_, arrival_);
                        market_data_->sender = (*this);
                        market_data_->sent   = ti.lower;
                        market_data_->symbol = m->order_details.symbol;

                        if(trade_) {
                            market_data_->trades.push_back({r.limit, r.quantity});
                        }
                        
                        if(b.ask().has_value()) {
                            market_data_->ask.push_back({b.ask().value(), 1});
                        }
                        if(b.bid().has_value()) {
                            market_data_->bid.push_back({b.bid().value(), 1});
                        }
                    }
                }
            }
            b.reports.clear();
            return ti.upper;
        }

        ///
        /// \brief  Create a market for a collection of tradeable properties.
        ///
        centralized_exchange( identity<centralized_exchange> i
                            , std::vector<markets::ticker> traded
                            )
        : agent(i)
        , market(i, traded)
        
        {
            books.reserve(traded.size());

            for(const auto &t: traded){
                positions.emplace(t, boost::container::flat_map<identity<agent>, position_report>());
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ESL_REGISTER_CALLBACK( logon_message
                                 , 0
                                 , [&]( auto m
                                      , auto ti
                                      , auto &s){return this->logon_participant(m, ti, s);}
                                 , "logon new participant");

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ESL_REGISTER_CALLBACK( logout_message
                                   , 0
                                   , [&]( auto m
                                        , auto ti
                                        , auto &s){return this->logout_participant(m, ti, s);}
                                   , "logout participant");


            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ESL_REGISTER_CALLBACK( market_data_request_message
                                   , 0
                                   , [&]( std::shared_ptr<market_data_request_message> m
                                        , simulation::time_interval ti
                                        , std::seed_seq &s){
                    (void)s;



                    for(const auto &i: m->instruments){
                        if(market_data_request_message::snapshot == m->type){
                            
                            // periodicity is set to max
                      
                            auto subscription_ =
                                market_data_subscriptions[i].find(m->sender);

                            centralized_exchange::data_subscription s;
                            s.depth       = m->depth;
                            s.start       = ti.lower;
                            s.periodicity = std::numeric_limits<simulation::time_point>::max();

                            if(market_data_subscriptions[i].end()
                               == subscription_) {
                                market_data_subscriptions[i].insert(
                                    {m->sender, s});
                            } else {
                                subscription_->second = s;
                            }
                               
                        }else if(market_data_request_message::cancel == m->type) {
                            auto instrument_ = market_data_subscriptions.find(i);
                            if(market_data_subscriptions.end() == instrument_) {
                                continue;
                            }
                            auto subscription_ = market_data_subscriptions[i].find(m->sender);
                            market_data_subscriptions[i].erase(subscription_);
                        }else if(market_data_request_message::stream == m->type) {
                            auto instrument_ = market_data_subscriptions.find(i);
                            if(market_data_subscriptions.end() == instrument_) {
                                continue;
                            }
                        
                            auto subscription_ = market_data_subscriptions[i].find(m->sender);

                              centralized_exchange::data_subscription s;
                              s.depth = m->depth;
                              s.start = ti.lower;
                              s.periodicity = 0;

                            if(market_data_subscriptions[i].end() == subscription_){
                                market_data_subscriptions[i].insert(
                                    {m->sender, s});
                            }else{
                                subscription_->second = s;
                            }
                        }
                    }






                    return ti.upper;
                }, "handle market data request");



            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
            ESL_REGISTER_CALLBACK( new_order_single_message
                                 , 0,
                [&](std::shared_ptr<new_order_single_message> m
                                      , simulation::time_interval ti
                                      , std::seed_seq &s)
                                    {
                                        return insert_order(m, ti, s);
                                    }
                                 , "handle new order");
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
            ESL_REGISTER_CALLBACK( cancel_order_message
                                 , 0,
                [&](std::shared_ptr<cancel_order_message> m
                                      , simulation::time_interval step
                                      , std::seed_seq &s) 
                                    { 
                                        cancel(m->symbol, m->order); 
                                        return step.upper;
                                    }
                                 , "cancel existing order");
        }

        centralized_exchange(const centralized_exchange &e) = default;

        virtual ~centralized_exchange() = default;

        ///
        ///
        ///
        /// \param order
        order_book::basic_book &
        insert(const limit_order &order)
        {
            auto i = books.find(order.symbol);

            if(i == books.end()) {
                // In the real world, this would trigger some kind of message to the trader saying the order is rejected.
                // Here in the simulation, we need to warn the modeller that an agent is placing orders that
                // can't be matched to any active market.
                throw esl::exception("Invalid ticker in limit order.");
            }
            i->second->insert(order);
            return *(i->second);
        }

        ///
        /// \param symbol
        /// \param identifier
        void cancel(const ticker &symbol,
                    const typename basic_book::order_identifier
                        identifier)
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
