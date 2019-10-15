/// \file   order_message.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-25
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
/// Oxford Martin School, University of Oxford
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
#ifndef ESL_WALRAS_ORDER_MESSAGE_HPP
#define ESL_WALRAS_ORDER_MESSAGE_HPP


#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>

#include <esl/economics/markets/order_message.hpp>


namespace esl::economics::markets::walras {
    ///
    /// \brief
    ///
    template<typename message_type_, std::uint64_t type_code_>
    struct walras_order_message
    : public esl::economics::markets::order_message<message_type_, type_code_>
    , public virtual demand_supply_function
    {
        walras_order_message(
            identity<agent> sender, identity<agent> recipient,
            simulation::time_point sent     = simulation::time_point(),
            simulation::time_point received = simulation::time_point())
        : markets::order_message<message_type_, type_code_>(sender, recipient,
                                                            sent, received)
        {}

        template<typename quote_type_, uint64_t any_>
        explicit walras_order_message(
            const markets::quote_message<quote_type_, any_> &q)
        : markets::order_message<message_type_, type_code_>(q)
        {}

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            typedef markets::order_message<message_type_, type_code_>
                order_message_type;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(order_message_type);
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(
                demand_supply_function);
        }
    };
}  // namespace esl::economics::markets::walras


#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {

    template<typename message_type_, uint64_t type_code_>
    struct is_mpi_datatype<esl::economics::markets::walras::
                               walras_order_message<message_type_, type_code_>>
    : public mpl::false_
    {

    };
} // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_WALRAS_ORDER_MESSAGE_HPP
