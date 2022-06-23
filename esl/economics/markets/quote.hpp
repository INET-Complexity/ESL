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
#include <utility>
#include <type_traits>

#include <esl/economics/exchange_rate.hpp>
#include <esl/economics/price.hpp>
#include <esl/exception.hpp>


namespace esl::economics::markets {

    ///
    /// \brief  A quote is a numerical value around which the market is
    ///         organised. Most commonly, this is a monetary value
    ///         (a price), but different markets may use different types of
    ///         quotes such as interest rates in a mortgage market, or a ratio
    ///         (exchange rate) when bartering.
    ///         
    ///         The quote class provides a generic interface, provided that 
    ///         all quote type variants allow conversion from and to 
    ///         floating point numbers at double precision.
    /// 
    ///         The lot size can be used to gain precision in quoting (setting prices). 
    ///         E.g., for a lot size of one and a `price`, the minimum increment is one cent in that currency.
    ///         If instead we set the lot size to 10, the minimum price increment is one tenth cent per unit. 
    ///         @TODO for raw exchange rates, the lot should match the base amount.        
    /// 
    struct quote
    {
    private:
        typedef std::uint32_t lot_t;
        constexpr void assert_equal_type_(const quote& other) const
        {
            if(type.index() != other.type.index()){
                throw esl::exception("quote types are different");
            }
        }

        /// 
        /// \brief  Until the rules for conversion have been detailed further, we don't accept lot size conversions
        /// 
        constexpr void assert_equal_lot_(const quote &other) const
        {
            if(lot != other.lot) {
                throw esl::exception("quotes have different lot sizes");
            }
        }

    public:
        /// 
        /// \brief  The number of items this quote applies to. The default is 1.
        /// 
        /// 
        lot_t lot;

        /// 
        /// \brief  The quote type variant. E.g. price, interest rate or exchange rate, or more exotically model volatility 
        /// 
        std::variant<price> type;

        /*explicit quote(const exchange_rate &er = exchange_rate(), lot_t lot = 1)
        : type(er)
        , lot(lot)
        {
            if(0 >= lot){
                throw esl::exception("lot size must be strictly positive");
            }
        }*/

        ///
        /// \param p
        /// \param lot
        explicit quote(const price &p = price(), lot_t lot = 1)
        : type(p)
        , lot(lot )
        {
            if(0 >= lot){
                throw esl::exception("lot size must be strictly positive");
            }
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

        ///
        /// \param o
        /// \return
        quote &operator=(const quote &o)
        { 
            this->type = o.type;
            this->lot  = o.lot;
            return *this;
        }

        ///
        /// \param other
        /// \return
        [[nodiscard]] constexpr bool operator == (const quote &other) const
        {
            assert_equal_type_(other);

            assert_equal_lot_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;

                if(auto *vp = std::get_if<variant_>(&other.type)  ){
                    return (lot * k) == ( (*vp) * other.lot );
                }
                throw esl::exception("quote variants do not match");
            }, type);
        }

        [[nodiscard]] constexpr bool operator != (const quote &other) const
        {
            assert_equal_type_(other);

            assert_equal_lot_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                if(auto *vp = std::get_if<variant_>(&other.type)  ){
                    return (lot * k) != ((*vp) * other.lot );
                }
                throw esl::exception("quote variants do not match");
            }, type);
        }


        [[nodiscard]] constexpr bool operator < (const quote &other) const
        {
            assert_equal_type_(other);

            assert_equal_lot_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                if(auto *vp = std::get_if<variant_>(&other.type)  ){
                    return (lot * k) < ((*vp) * other.lot );
                }
                throw esl::exception("quote variants do not match");
            }, type);
        }

        [[nodiscard]] constexpr bool operator > (const quote &other) const
        {
            assert_equal_type_(other);

            assert_equal_lot_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                //return (lot * k) > (std::get<variant_>(other.type) * other.lot );
                if(auto *vp = std::get_if<variant_>(&other.type)  ){
                    return (lot * k) > ((*vp) * other.lot );
                }
                throw esl::exception("quote variants do not match");
            }, type);
        }

        [[nodiscard]] constexpr bool operator <= (const quote &other) const
        {
            assert_equal_type_(other);

            assert_equal_lot_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                //return (lot * k) <= (std::get<variant_>(other.type) * other.lot );
                if(auto *vp = std::get_if<variant_>(&other.type)  ){
                    return (lot * k) <= ((*vp) * other.lot );
                }
                throw esl::exception("quote variants do not match");
            }, type);
        }

        [[nodiscard]] constexpr bool operator >= (const quote &other) const
        {
            assert_equal_type_(other);

            return std::visit([&] (const auto& k) {
                using variant_ = std::decay_t<decltype(k)>;
                //return (lot * k) >= (std::get<variant_>(other.type) * other.lot );
                if(auto *vp = std::get_if<variant_>(&other.type)  ){
                    return (lot * k) >= ((*vp) * other.lot );
                }
                throw esl::exception("quote variants do not match");
            }, type);
        }


        [[nodiscard]] quote operator + (const quote &addend) const
        { 
            assert_equal_type_(addend);
            assert_equal_lot_(addend);

            auto result_ = std::visit( [&](const auto &augend) 
                {
                    return augend + std::get<std::decay_t<decltype(augend)>>(addend.type);
                }, type);

            return quote(result_, lot);
        }

        [[nodiscard]] quote operator - (const quote &subtrahend) const
        {
            assert_equal_type_(subtrahend);
            assert_equal_lot_(subtrahend);

            auto result_ = std::visit( [&](const auto &minuend) 
                {
                    return minuend - std::get<std::decay_t<decltype(minuend)>>(subtrahend.type);
                }, type);

            return quote(result_, lot);
        }

        [[nodiscard]] quote operator * (const quote &multiplicand) const
        {
            assert_equal_type_(multiplicand);
            assert_equal_lot_(multiplicand);

            auto result_ = std::visit(
                [&](const auto &multiplier) {
                    return multiplier * std::get<std::decay_t<decltype(multiplier)>>(multiplicand.type);
                }, type);

            return quote(result_, lot);
        }

        [[nodiscard]] quote operator / (const quote &denominator) const
        {
            assert_equal_type_(denominator);
            assert_equal_lot_(denominator);

            auto result_ = std::visit(
                [&](const auto &numerator) {
                    return numerator / std::get<std::decay_t<decltype(numerator)>>(denominator.type);
                }, type);

            return quote(result_, lot);
        }
        
        ////////////

        [[nodiscard]] quote operator+(double addend) const
        {
            auto result_ = std::visit(
                [&](const auto &augend) {
                    return augend + addend;
                },
                type);

            return quote(result_, lot);
        }

        [[nodiscard]] quote operator-(double subtrahend) const
        {
            auto result_ = std::visit(
                [&](const auto &minuend) {
                    return minuend - subtrahend;
                },
                type);

            return quote(result_, lot);
        }

        [[nodiscard]] quote operator*(double multiplicand) const
        {
            auto result_ = std::visit(
                [&](const auto &multiplier) {
                    return multiplier * multiplicand;
                },
                type);

            return quote(result_, lot);
        }

        [[nodiscard]] quote operator/(double denominator) const
        {
            auto result_ = std::visit(
                [&](const auto &numerator) {
                    return numerator / denominator;
                },
                type);

            return quote(result_, lot);
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
                //if(auto *vp = std::get_if<exchange_rate>(&type)  ){
                //    archive << boost::serialization::make_nvp(
                //        "exchange_rate", (vp));
                //    break;
                //}
            case 1:
                if(auto *vp = std::get_if<price>(&type)  ){
                    archive << boost::serialization::make_nvp(
                        "price", *vp);
                    break;
                }
            default:
                throw esl::exception("variant quote not supported");
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
                //exchange_rate re;
                //archive >> boost::serialization::make_nvp("exchange_rate", re);
                //type.emplace<0>(re);
            } else if(1 == index_) {
                price p;
                archive >> boost::serialization::make_nvp("price", p);
                type.emplace<price>(p);
            }
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            boost::serialization::split_member(archive, *this, version);
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

        [[nodiscard]] std::string representation() const
        {
            std::stringstream stream_;
            stream_ << *this;
            return stream_.str();
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
    {

    };
}}      // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_QUOTE_HPP
