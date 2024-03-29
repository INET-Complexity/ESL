/// \file   owner.hpp
///
/// \brief  Allows an agent to hold property of a specific type, and allows one
///         to write specialised processing of property of this type
///
/// \authors    Maarten P. Scholl
/// \date       2019-01-21
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
#ifndef ESL_OWNER_HPP
#define ESL_OWNER_HPP

#include <stdexcept>

#include <boost/type_traits/is_virtual_base_of.hpp>

#include <esl/agent.hpp>
#include <esl/data/log.hpp>
#include <esl/economics/accounting/inventory.hpp>
#include <esl/interaction/transfer.hpp>

namespace esl::law {
    ///
    /// \brief  An owner<t> can have some property t, and it can process
    ///         transactions relating to those properties. For
    ///         specific property types, the class can be specialised so that
    ///         diverse transactions (and messages) can be implemented a single
    ///         modular class that can be added on to existing agents.
    ///
    /// \tparam property_t_ a derived class of property. Is expected to have a
    ///         std::hash<property_t_> implementation
    template<typename property_t_>
    struct owner;

    struct property;

    ///
    /// \brief  The base class transfers properties of any type.
    ///
    template<>
    struct owner<property>
    : public virtual agent
    , public identifiable_as<owner<property>>
    {
    public:
        property_map<quantity> inventory;

        explicit owner(const identity<owner<property>> &i =
                           identity<owner<property>>())
        : agent(i), identifiable_as<owner<property>>(), inventory()
        {
            auto callback_ = [this](auto msg,
                                    simulation::time_interval ti,
                                    std::seed_seq &seed) {
                (void)seed;
                if(msg->transferor == msg->transferee){
                   return simulation::time_point(ti.upper);
                }

                if(this->identifier == msg->transferor) {
                    try {
                        LOG(trace) << this->name() << " sends " << msg->transferred << std::endl;
                        msg->transferred.erase_from(inventory);
                    }catch(const esl::economics::accounting::insufficent_inventory &e){
                        // ...
                        LOG(errorlog) << this->name() << " (" << identifier  << ", " << msg->transferred <<") " << e.what() << std::endl;
                        throw e;
                    }
                } else if(this->identifier == msg->transferee) {
                    LOG(trace) << this->name() << " receives " << msg->transferred << std::endl;
                    msg->transferred.insert_into(inventory);
                } else {
                    LOG(notice)
                        << "message recipient " << identifier
                        << " is not a party to the transfer between transferee "
                        << msg->transferee << " and transferor "
                        << msg->transferor << std::endl;
                }
                return simulation::time_point(ti.upper);
            };


            ESL_REGISTER_CALLBACK(interaction::transfer,
                                  50,
                                  callback_,
                                  "process interaction::transfer");
        }

        virtual ~owner() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(agent);
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(
                identifiable_as<owner<property>>);
            archive &BOOST_SERIALIZATION_NVP(inventory);
        }
    };

    namespace detail {

        template<bool fungible_, typename property_t_>
        struct filter_inserter;

        template<typename property_t_>
        struct filter_inserter<false, property_t_>
        {
            void insert(economics::accounting::inventory_filter<property_t_>
                            &properties,
                        std::shared_ptr<property_t_>
                            p,
                        const quantity &q)
            {
                assert(q == quantity(1));
                (void)q;
                properties.insert(p);
            }

            void erase(economics::accounting::inventory_filter<property_t_>
                           &properties,
                       std::shared_ptr<property_t_>
                           p,
                       const quantity &q)
            {
                assert(q == quantity(1));
                (void)q;
                properties.erase(p);
            }
        };

        template<typename property_t_>
        struct filter_inserter<true, property_t_>
        {
            void insert(economics::accounting::inventory_filter<property_t_>
                            &properties,
                        std::shared_ptr<property_t_> p,
                        const quantity &q)
            {
                properties.insert(p, q);
            }

            void erase(economics::accounting::inventory_filter<property_t_>
                           &properties,
                       std::shared_ptr<property_t_>
                           p,
                       const quantity &q)
            {
                properties.erase(p, q);
            }
        };
    }  // namespace detail

    template<typename property_t_>
    struct owner
    : public virtual owner<property>
    , public identifiable_as<owner<property_t_>>
    {
        static_assert( boost::is_base_of<property, property_t_>::value
                    || boost::is_virtual_base_of<property, property_t_>::value,
                      "filtered type must derive from property");

        ///
        /// \brief the legal_person's holdings filtered by this particular
        ///         property's type.
        ///
        economics::accounting::inventory_filter<property_t_> properties;

        using owner<property>::owner;

        explicit owner(const identity<owner<property>> &i =
                           identity<owner<property>>())
        : owner<property>(i)
        , identifiable_as<owner<property_t_>>()
        , properties()
        {
            auto process_transfer_ = [this](auto msg,
                                            simulation::time_interval ti,
                                            std::seed_seq &seed) {
                (void)seed;
                if((*this).identifier == msg->transferor) {
                    // this process is duplicated several times for every
                    // inherited owner<p>, as we are reliant on dynamic type
                    // information
                    for(const auto &[i, q] : msg->transferred.items) {
                        auto d = std::dynamic_pointer_cast<property_t_>(i);
                        if(!d) {
                            continue;
                        }
                        //LOG(trace) << typeid(property_t_).name() << " (" << i->identifier << ") " <<  identifier << " is sending "<< msg->transferred.items << " to " << msg->transferee << std::endl;

                        try {
                            detail::filter_inserter<
                                std::is_base_of<economics::fungible,
                                                property_t_>::value,
                                property_t_>()
                                .erase(properties, d, q);
                        }catch(economics::accounting::insufficent_inventory &e){
                            LOG(errorlog) << name() << " " << e.what() << std::endl;
                            throw e;
                        }
                    }

                } else if((*this).identifier == msg->transferee) {
                    for(const auto &[i, q] : msg->transferred.items) {
                        auto d = std::dynamic_pointer_cast<property_t_>(i);
                        if(!d) {
                            continue;
                        }
                        //LOG(trace) << typeid(property_t_).name() << " (" << i->identifier << ") " <<  msg->transferee << " is receiving "<< msg->transferred.items << " from " << msg->transferor << std::endl;
                        detail::filter_inserter<
                            std::is_base_of<economics::fungible,
                                            property_t_>::value,
                            property_t_>()
                            .insert(properties, d, q);
                    }
                }else{
                    //LOG(notice) << "message recipient is not a party to the transfer" << std::endl;
                }
                return ti.upper;
            };

            std::stringstream description_;
            description_ << "process interaction::transfer(";
            description_ << typeid(property_t_).name();
            description_ << ")";

            ESL_REGISTER_CALLBACK(interaction::transfer,
                                  50,
                                  process_transfer_,
                                  description_.str());
        }


        void take(std::shared_ptr<property_t_> property, quantity amount)
        {
            this->properties.insert(property, amount);
            esl::law::owner<esl::law::property>::inventory.insert(
                std::make_pair(property, amount));
        }


        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive
                &boost::serialization::base_object<owner<esl::law::property>>(
                    *this);

            // archive & BOOST_SERIALIZATION_NVP(properties);
        }
    };
}  // namespace esl::law

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<typename property_t_>
    struct is_mpi_datatype<esl::law::owner<property_t_>> : mpl::false_
    { };
}  // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_OWNER_HPP
