/// \file   dividend.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-03-19
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
#ifndef ESL_DIVIDEND_HPP
#define ESL_DIVIDEND_HPP

#include <map>
#include <set>
#include <tuple>
#include <utility>

#include <esl/economics/currencies.hpp>
#include <esl/economics/finance/share_class.hpp>
#include <esl/economics/price.hpp>
#include <esl/interaction/message.hpp>

namespace esl::economics::finance {

    struct dividend_policy
    {
        ///
        ///
        ///
        simulation::time_point announcement_date;

        // On the ex-dividend date shareholders contact the company to receive
        // dividends. In the real world, there is also a record date and the
        // time between ex-dividend and record date is set to allow for
        // communication overhead (various business day rules exist). For now,
        // this aspect is ignored
        simulation::time_point ex_dividend_date;
        // record date (not used)
        // time_point record_date;

        ///
        simulation::time_interval dividend_period;

        /// payable date
        simulation::time_point payable_date;

        /// \brief  Payment currency
        iso_4217 dividend_currency;

        // total dividends
        // per share type, shares outstanding, dividend per share
        std::map<share_class, std::tuple<std::uint64_t, price>> dividend_per_share;

        dividend_policy(
            simulation::time_point announcement_date =
                simulation::time_point(0),
            simulation::time_point ex_dividend_date = simulation::time_point(0),
            const simulation::time_interval &dividend_period =
                {simulation::time_point(0), simulation::time_point(0)},
            simulation::time_point payable_date = simulation::time_point(0),
            const iso_4217 &dividend_currency   = currencies::USD,
            const std::map<share_class, std::tuple<std::uint64_t, price>>
                &dividend_per_share = {});

        ///
        /// \brief  total dividends payed out by the company
        /// \return
        ///
        price total_dividends() const;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_NVP(announcement_date);
            archive &BOOST_SERIALIZATION_NVP(ex_dividend_date);
            archive &BOOST_SERIALIZATION_NVP(dividend_period);
            archive &BOOST_SERIALIZATION_NVP(payable_date);
            archive &BOOST_SERIALIZATION_NVP(dividend_currency);
            archive &BOOST_SERIALIZATION_NVP(dividend_per_share);
        }
    };


    // struct company;
    // struct shareholder;

    ///
    ///
    ///
    struct dividend_announcement_message
    : interaction::message<dividend_announcement_message,
                           esl::interaction::library_message_code<0x0F00>()>
    {
        dividend_policy policy;

        dividend_announcement_message(
            identity<agent> issuer        = identity<agent>(),
            identity<agent> receiver      = identity<agent>(),
            const dividend_policy &policy = dividend_policy())
        : interaction::message<
            dividend_announcement_message,
            esl::interaction::library_message_code<0x0F00>()>(issuer, receiver)
        , policy(policy)
        {}

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            boost::serialization::make_nvp(
                "message",
                boost::serialization::base_object<
                    message<dividend_announcement_message,
                            esl::interaction::library_message_code<0x0F00>()>>(
                    *this));
            archive &BOOST_SERIALIZATION_NVP(policy);
        }
    };


    ///
    /// Agents communicate to the company that they may have rights to the
    /// dividend payout. For
    ///
    struct dividend_record
    : interaction::message<dividend_record,
                           esl::interaction::library_message_code<0x0F01>()>
    {
        ///
        /// \brief  For optimization purposes, we allow the agents to compute
        /// their own payout and simply bill the company,
        ///         rather than first notifying them about the amount they own.
        std::map<share_class, std::uint64_t> shares_on_record;
        // set<share> shares_on_record;

        explicit dividend_record(
            identity<agent> sender                          = identity<agent>(),
            identity<agent> recipient                       = identity<agent>(),
            std::map<share_class, std::uint64_t> shares_on_record = {}
            //, set<share> shares_on_record = {}
            ,
            simulation::time_point sent     = simulation::time_point(),
            simulation::time_point received = simulation::time_point())
        : interaction::message<
            dividend_record,
            esl::interaction::library_message_code<0x0F01>()>(
            std::move(sender),
            std::move(recipient),
            sent,
            received)
        , shares_on_record(std::move(shares_on_record))
        {}


        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            boost::serialization::make_nvp(
                "message",
                boost::serialization::base_object<
                    message<dividend_record,
                            esl::interaction::library_message_code<0x0F01>()>>(
                    *this));

            archive &BOOST_SERIALIZATION_NVP(shares_on_record);
        }
    };

}  // namespace esl::economics::finance

#endif  // ESL_DIVIDEND_HPP
