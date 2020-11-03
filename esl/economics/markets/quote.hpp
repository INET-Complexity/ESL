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

#include <esl/economics/exchange_rate.hpp>
#include <esl/economics/price.hpp>


namespace esl::economics::markets {

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
    private:
        constexpr void assert_equal_type_(const quote& other) const
        {
            if(type.index() != other.type.index()){
                throw std::logic_error("comparing quotes of different types");
            }
        }
    public:
        std::variant<exchange_rate, price> type;

        uint64_t lot;

        explicit quote(const exchange_rate &er = exchange_rate(),
                       uint64_t lot            = 1)
        : type(er)
        , lot(lot)
        {
            assert(lot > 0);
        }

        ///
        /// \param p
        /// \param lot
        explicit quote(const price &p, uint64_t lot = 1)
        : type(p)
        , lot(lot )
        {
            assert(lot > 0);
        }

        explicit quote(double f, const quote &similar)
        {
            *this = std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                return quote(variant_(f, k), similar.lot);
                }, similar.type);
        }


        quote(const quote &q)
        : type(q.type)
        , lot(q.lot)
        {
            assert(lot > 0);
        }

        ///
        /// \brief  When converting to floating point, we divide by the lot size
        ///         so that we get the quote per unit of goods.
        ///
        /// \tparam floating_point_t_
        /// \return
        template<typename floating_point_t_>
        explicit operator floating_point_t_() const
        {
            return std::visit(
                [this](auto &&arg) { return floating_point_t_(arg) / lot; },
                type);
        }

        quote &operator = (const quote &o) = default;

        constexpr [[nodiscard]] bool operator == (const quote &other) const
        {
            assert_equal_type_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                return (lot * k) == (std::get<variant_>(other.type) * other.lot );
            }, type);
        }

        constexpr [[nodiscard]] bool operator != (const quote &other) const
        {
            assert_equal_type_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                return (lot * k) != (std::get<variant_>(other.type) * other.lot );
            }, type);
        }


        constexpr [[nodiscard]] bool operator < (const quote &other) const
        {
            assert_equal_type_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                return (lot * k) < (std::get<variant_>(other.type) * other.lot );
            }, type);
        }

        constexpr [[nodiscard]] bool operator > (const quote &other) const
        {
            assert_equal_type_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                return (lot * k) > (std::get<variant_>(other.type) * other.lot );
            }, type);
        }

        constexpr [[nodiscard]] bool operator <= (const quote &other) const
        {
            assert_equal_type_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                return (lot * k) <= (std::get<variant_>(other.type) * other.lot );
            }, type);
        }

        constexpr [[nodiscard]] bool operator >= (const quote &other) const
        {
            assert_equal_type_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                return (lot * k) >= (std::get<variant_>(other.type) * other.lot );
            }, type);
        }


        template<class archive_t>
        void save(archive_t &archive, const unsigned int version) const
        {
            (void)version;

            archive << BOOST_SERIALIZATION_NVP(lot);
            size_t index_ = type.index();
            archive &BOOST_SERIALIZATION_NVP(index_);
            switch(index_) {
            case 0:
                archive << boost::serialization::make_nvp(
                    "exchange_rate", std::get<exchange_rate>(type));
                break;
            case 1:
                archive << boost::serialization::make_nvp(
                    "price", std::get<price>(type));
                break;

            default:
                throw std::logic_error("variant quote not supported");
            }
        }

        template<class archive_t>
        void load(archive_t &archive, const unsigned int version)
        {
            (void)version;

            archive >> BOOST_SERIALIZATION_NVP(lot);

            size_t index_ = 0;
            archive >> BOOST_SERIALIZATION_NVP(index_);
            if(0 == index_) {
                exchange_rate re;
                archive >> boost::serialization::make_nvp("exchange_rate", re);
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


        std::ostream &operator << (std::ostream &stream) const
        {
            stream << lot << '@';
            std::visit([&](const auto &elem)
                       {
                           stream << elem;
                       },
                       type);
            return stream;
        }

        friend std::ostream &operator << (std::ostream &stream, const quote &q)
        {
            stream << q.lot << '@';
            std::visit([&](const auto &elem)
                       {
                           stream << elem;
                       },
                       q.type);
            return stream;
        }
    };

}  // namespace esl::economics


#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost { namespace mpi {
    template<>
    struct is_mpi_datatype<esl::economics::markets::quote>
    : public boost::mpl::true_//is_mpi_datatype<std::variant<esl::economics::exchange_rate
             //                             , esl::economics::price>>::value
    { };
}}      // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_QUOTE_HPP
