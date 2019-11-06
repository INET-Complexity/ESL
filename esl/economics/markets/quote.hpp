/// \file   walras.hpp
///
/// \brief  A quote is the variable along which trade negotation takes place
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-04
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
#ifndef ESL_QUOTE_HPP
#define ESL_QUOTE_HPP

#include <variant>

#include <type_traits>

#include <esl/economics/price.hpp>
#include <esl/economics/exchange_rate.hpp>


namespace esl::economics {

    ///
    /// \brief  A quote is a numerical value around which the market is
    ///     organised. Most commonly, this is a monetary value
    ///         (a price), but different markets may use different types of
    ///         quotes such as interest rates in a mortgage market, or a ratio
    ///         (exchange rate) when bartering.
    ///
    ///
    struct quote
    {
        std::variant<exchange_rate, price> type;

        explicit quote(const exchange_rate &er = exchange_rate())
        : type(er)
        {}

        explicit quote(const price &p)
        : type(p)
        {}

        quote(const quote &q)
        : type(q.type)
        {

        }

        template<typename floating_point_t_>
        explicit operator floating_point_t_() const
        {
            return std::visit([](auto &&arg) { return floating_point_t_(arg); },
                              type);
        }

        template<class archive_t>
        void save(archive_t &archive, const unsigned int version) const
        {
            (void)version;

            size_t index_ = type.index();
            archive &BOOST_SERIALIZATION_NVP(index_);
            switch(index_) {
            case 0:
                archive << boost::serialization::make_nvp("exchange_rate",
                std::get<exchange_rate>(type));
                break;
            case 1:
                archive << boost::serialization::make_nvp("price",
                                                          std::get<price>(type));
                break;
            }
        }


        template<class archive_t>
        void load(archive_t &archive, const unsigned int version)
        {
            (void)version;

            size_t index_ = 0;
            archive >> BOOST_SERIALIZATION_NVP(index_);
            if(0 == index_) {
                exchange_rate re;
                archive >> boost::serialization::make_nvp("exchange_rate",
                re);
                type.emplace<0>(re);
            } else if(1 == index_) {
                price p;
                archive >> boost::serialization::make_nvp("price", p);
                type.emplace<1>(p);
            }
        }


        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            boost::serialization::split_member(archive, *this, version);
        }
    };

}  // namespace esl::economics


#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost { namespace mpi {
    template<>
    struct is_mpi_datatype<esl::economics::quote> : public mpl::false_
    {};
}}      // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_QUOTE_HPP
