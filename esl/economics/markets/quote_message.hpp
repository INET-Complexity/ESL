/// \file   quote_message.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-08
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
#ifndef ESL_QUOTE_MESSAGE_HPP
#define ESL_QUOTE_MESSAGE_HPP

#include <esl/interaction/message.hpp>
#include <esl/law/property_collection.hpp>
#include <esl/economics/markets/quote.hpp>
#include <esl/economics/markets/indication.hpp>


namespace esl::economics::markets {
    ///
    /// \brief
    /// \tparam message_type_
    /// \tparam type_code_
    template<typename message_type_, uint64_t type_code_>
    struct quote_message
    : public interaction::message<message_type_, type_code_>
    {
        ///
        /// \brief  Quotes for each property
        ///
        law::property_map<quote> proposed;

        ///
        /// \brief  Whether the sender is bound to deliver on the offered
        ///         quote (see @indication).
        ///
        indication binding;

        ///
        /// \brief
        ///
        explicit quote_message(
            identity<agent> sender            = identity<agent>(),
            identity<agent> recipient         = identity<agent>(),
            law::property_map<quote> proposed = {},
            indication binding                = indication::firm,
            simulation::time_point sent       = simulation::time_point(),
            simulation::time_point received   = simulation::time_point())
        : interaction::message<message_type_, type_code_>(sender, recipient,
                                                          sent, received)
        , proposed(move(proposed))
        , binding(binding)
        {

        }

        virtual ~quote_message() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &make_nvp(
                "interaction〈message_type_,type_code_〉",
                boost::serialization::base_object<
                    interaction::message<message_type_, type_code_>>(*this));
            archive &BOOST_SERIALIZATION_NVP(proposed);
            archive & BOOST_SERIALIZATION_NVP(binding);
        }
    };
}  // namespace esl::economics::markets


#ifdef WITH_MPI
#include <boost/mpi/datatype.hpp>

namespace boost::mpi {

    template<typename message_type_, uint64_t type_code_>
    struct is_mpi_datatype<
        esl::economics::markets::quote_message<message_type_, type_code_>>
        : public mpl::false_
    {

    };
}   // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_QUOTE_MESSAGE_HPP
