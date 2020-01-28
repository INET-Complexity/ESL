/// \file   inventory.hpp
///
/// \brief
/// \warning    Unused
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-28
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
#ifndef ESL_INVENTORY_HPP
#define ESL_INVENTORY_HPP

#include <boost/serialization/unordered_map.hpp>

#include <esl/economics/accounting/standard.hpp>
#include <esl/economics/cash.hpp>
#include <esl/economics/currencies.hpp>
#include <esl/law/property.hpp>
#include <esl/quantity.hpp>
#include <esl/law/property_collection.hpp>


namespace esl::economics::accounting {


    struct insufficent_inventory
    : public std::exception
    {
    public:
        const quantity before;

        const quantity withdrawal;

        explicit insufficent_inventory(quantity before, quantity withdrawal)
        : before(before), withdrawal(withdrawal)
        {}

        ///
        /// \return a human-readable message that describes the missing item and
        /// quantity
        [[nodiscard]] const char *what() const noexcept override
        {
            std::stringstream stream_;
            return stream_.str().c_str();
        }
    };


    struct fractional_infungible
    : public std::exception
    {
    public:
        const quantity invalid;

        explicit fractional_infungible(quantity q) : invalid(q)
        {}

        const char *what() const throw()
        {
            return "attempt to divide infungible property";
        }
    };


    struct duplicate_infungible
    : public std::exception
    {
    public:
        const quantity invalid;

        explicit duplicate_infungible(quantity q) : invalid(q)
        {}

        const char *what() const throw()
        {
            return "attempt to duplicate infungible property";
        }
    };


    ///
    /// \brief  This container is used to map specific property types to the
    /// approrpriate datastructures. The underlying
    ///         datastructures can be changed later behind this static
    ///         interface, for example for performance reasons (example:
    ///         vector<> may be faster than unordered_set, to be confirmed in
    ///         testing.
    ///
    /// \tparam property_t_
    /// \tparam fungible_   whether or not the property is a fungible property.
    template<typename property_t_, bool fungible_>
    struct inventory_by_fungibility;


    ///
    /// \tparam property_t_
    template<typename property_t_>
    struct inventory_by_fungibility<property_t_, true>
    {
        law::property_filter_map<property_t_, quantity> items;

        inventory_by_fungibility() : items()
        {}

        inventory_by_fungibility(property_t_ item, quantity q) : items()
        {
            items.insert(make_pair(item, q));
        }


        // std::enable_if<std::is_base_of<asset, property_t_>::value, price>
        price value(const accounting::standard &a)
        {
            // throw not_implemented_exception("price value(const standard
            // &a)");
            auto result_ = economics::cash(a.reporting_currency).value(0);
            for(auto [k, v] : items) {
                auto unit_     = k->value(a);
                auto multiple_ = (unit_.value * v.amount) / v.basis;
                // auto q  = quantity(multiple_, unit_.valuation.denominator);
                auto p = price(multiple_, unit_.valuation);
                result_ += p;
            }
            return result_;
        }

        void insert(std::shared_ptr<property_t_> item, quantity q)
        {
            auto iterator_ = items.find(item);
            if(items.end() == iterator_) {
                items.insert(std::make_pair(item, q));
            } else {
                iterator_->second += q;
            }
        }

        void insert(const inventory_by_fungibility<property_t_, true> &added)
        {
            for(const auto &[k, v] : added.items) {
                insert(k, v);
            }
        }

        void erase(std::shared_ptr<property_t_> item, const quantity &q)
        {
            auto iterator_ = items.find(item);

            if(items.end() == iterator_ && q.amount > 0) {
                throw insufficent_inventory(quantity(0), q);
            }

            if(iterator_->second < q) {
                throw insufficent_inventory(iterator_->second, q);
            }

            iterator_->second -= q;
        }

        void erase(const inventory_by_fungibility<property_t_, true> &removed)
        {
            for(const auto &[k, v] : removed.items) {
                erase(k, v);
            }
        }


        void insert_into(law::property_map<quantity> &m) const
        {
            for(auto [k, v] : items) {
                auto i = m.find(k);
                if(m.end() == i) {
                    m.insert({k, v});
                } else {
                    i->second += v;
                }
            }
        }


        void erase_from(law::property_map<quantity> &m) const
        {
            for(auto [k, v] : items) {
                auto i = m.find(k);
                if(m.end() == i) {
                    std::cout << "when looking for property " << typeid(property_t_).name() << " id: "<< k->identifier << " no entry was found" << std::endl;
                    throw insufficent_inventory(quantity(0), quantity(1));
                } else if(i->second < v) {
                    std::cout << "when looking for property " << typeid(property_t_).name() << " id: "<< k->identifier << " there were insufficient items" << std::endl;
                    throw insufficent_inventory(i->second, v);
                } else if(i->second == v) {
                    m.erase(k);
                } else {
                    i->second -= v;
                }
            }
        }

        friend std::ostream &
        operator<<(std::ostream &o, inventory_by_fungibility<property_t_, true> &i)
        {
            o << "{ ";
            for(auto [p, q] : i.items) {
                o << p->name() << ": " << q << ", ";
            }
            o << "}";
            return o;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_NVP(items);
        }
    };

    ///
    /// \brief  An inventory of infungible (unique) property.
    ///
    /// \tparam property_t_
    template<typename property_t_>
    struct inventory_by_fungibility<property_t_, false>
    {
        law::property_filter_set<property_t_> items;

        int FUNG = 123;

        inventory_by_fungibility() = default;

        void insert(std::shared_ptr<property_t_> value)
        {
            items.insert(value);
        }

        ///
        /// \brief  Adds an overload to insert to insertentire collections of
        /// property \param values
        void insert(const law::property_set &values)
        {
            items.insert(values.begin(), values.end());
        }

        void erase(std::shared_ptr<property_t_> value)
        {
            items.erase(value);
        }

        ///
        /// \brief  Adds an overload to erase to insert entire collections of
        /// property \param values
        void erase(const law::property_set &values)
        {
            items.erase(values.begin(), values.end());
        }

        void insert_into(law::property_map<quantity> &m) const
        {
            for(auto k : items) {
                auto i = m.find(k);
                if(m.end() == i) {
                    m.insert({k, quantity(1)});
                } else {
                    throw duplicate_infungible(i->second + 1);
                }
            }
        }

        void erase_from(law::property_map<quantity> &m) const
        {
            for(auto k : items) {
                auto i = m.find(k);
                if(m.end() == i || i->second.amount == 0) {
                    throw insufficent_inventory(quantity(0), quantity(1));
                } else {
                    // if(i->second.amount > 1){
                    //    throw duplicate_infungible();
                    //}
                    m.erase(k);
                }
            }
        }


        ///
        /// \return
        std::enable_if<std::is_base_of<asset, property_t_>::value, price>
        value()
        {
            auto default_currency = currencies::USD;  // TODO
            auto result_          = price(0, default_currency);

            for(const auto &element : items) {
                result_ += element;
            }

            return result_;
        }

        friend std::ostream &
        operator<<(std::ostream &o, inventory_by_fungibility<property_t_, false> &i)
        {
            o << "{ ";
            for(auto p : i.items) {
                o << p->name() << ", ";
            }
            o << "}";
            return o;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_NVP(items);
        }
    };

    template<typename property_t_>
    using inventory_filter = inventory_by_fungibility<
        property_t_, std::is_same<esl::law::property, property_t_>::value
                         || std::is_base_of<esl::economics::fungible, property_t_>::value>;


}  // namespace esl::economics::accounting






#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<typename property_t_>
    struct is_mpi_datatype<esl::economics::accounting::inventory_filter<property_t_>>
        : public mpl::false_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI



#endif  // PROJECT_INVENTORY_HPP
