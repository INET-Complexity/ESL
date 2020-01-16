/// \file   price.hpp
///
/// \brief  The monetary value of something, as quoted in an offer, or agreed on in a transaction
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-28
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking, Oxford Martin School, University of Oxford
///
///             Licensed under the Apache License, Version 2.0 (the "License");
///             you may not use this file except in compliance with the License.
///             You may obtain a copy of the License at
///
///                 http://www.apache.org/licenses/LICENSE-2.0
///
///             Unless required by applicable law or agreed to in writing, software
///             distributed under the License is distributed on an "AS IS" BASIS,
///             WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
///             See the License for the specific language governing permissions and
///             limitations under the License.
///
///             You may obtain instructions to fulfill the attribution requirements in CITATION.cff
///
#ifndef ESL_PRICE_HPP
#define ESL_PRICE_HPP

#include <iomanip>
#include <cmath>

#include <esl/economics/iso_4217.hpp>


namespace esl::economics {

    namespace detail {
        constexpr void assert_equal_currencies(const iso_4217 &first,
                                               const iso_4217 &second)
        {
            if(first != second) {
                throw std::invalid_argument("comparing price of with currencies");
            }
        }
    }

    ///
    /// A price is a quote in terms of monetary value.
    ///
    struct price
    {
        std::int64_t value;
        iso_4217 valuation;

        explicit constexpr price(std::int64_t value,
                                 iso_4217 valuation)  // = default_currency)
        : value(value), valuation(valuation)
        {

        }

        constexpr price(const price &p)
        : price(p.value, p.valuation)
        {

        }

        //price &operator = (const price &p)
        //{
            //value = p.value;
            //valuation = p.valuation;
        //}

        [[nodiscard]] constexpr bool operator==(const price &operand) const
        {
            return (value == operand.value) && (valuation == operand.valuation);
        }

        [[nodiscard]] constexpr bool operator!=(const price &operand) const
        {
            return (value != operand.value) || (valuation != operand.valuation);
        }

        [[nodiscard]] constexpr bool operator < (const price &operand) const
        {
            detail::assert_equal_currencies(valuation, operand.valuation);
            return value < operand.value;
        }

        [[nodiscard]] constexpr bool operator<=(const price &operand) const
        {
            return (*this < operand) || (*this == operand);
        }

        [[nodiscard]] constexpr bool operator>(const price &operand) const
        {
            detail::assert_equal_currencies(valuation, operand.valuation);
            return value > operand.value;
        }

        [[nodiscard]] constexpr bool operator>=(const price &operand) const
        {
            return (*this > operand) || (*this == operand);
        }

        [[nodiscard]] constexpr price operator+(const price &operand) const
        {
            assert(valuation == operand.valuation);
            return price(value + operand.value, valuation);
        }

        constexpr price &operator+=(const price &operand)
        {
            assert(valuation == operand.valuation);
            value += operand.value;
            return *this;
        }

        [[nodiscard]] constexpr price operator-(const price &operand) const
        {
            assert(valuation == operand.valuation);
            return price(value - operand.value, valuation);
        }

        constexpr price &operator-=(const price &operand)
        {
            assert(valuation == operand.valuation);
            value -= operand.value;
            return *this;
        }

        template<typename integer_t_>
        [[nodiscard]] constexpr price operator*(const integer_t_ &scalar) const
        {
            static_assert(std::is_integral<integer_t_>::value);
            return price(value * scalar, valuation);
        }

        [[nodiscard]] constexpr friend price operator*(const price &p, const uint64_t &operand)
        {
            return price(p.value * operand, p.valuation);
        }

        [[nodiscard]] constexpr friend price operator*(const uint64_t &operand, const price &p)
        {
            return p * operand;
        }

        [[nodiscard]] explicit operator double() const
        {
            return double(value) / valuation.denominator;
        }

        friend std::ostream &operator<<(std::ostream &o, const price &p)
        {
            std::ios_base::fmtflags flags_(o.flags());
            int precision_ = ceil(log10(p.valuation.denominator));
            o << p.valuation << ' ' << std::fixed
              << std::setprecision(precision_) << double(p);
            o.flags(flags_);
            return o;
        }

        //friend std::istream &operator>>(std::istream &i, const price &p)
        //{
        //    std::array<char, 3> iso_currency_code_;
        //    i.read(iso_currency_code_.data(), iso_currency_code_.size());
        //    std::int64_t amount_;
        //    i >> amount_;
        //    p = price(amount_, iso_currency_code_);
        //    return i;
        //}

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_NVP(value);
            archive &boost::serialization::make_nvp(
                "valuation", const_cast<iso_4217 &>(valuation));
        }
    };
}

#ifdef WITH_MPI
namespace boost {
    namespace mpi {
        template<>
        struct is_mpi_datatype<esl::economics::price>
        : public mpl::true_
        {};
    }
}
#endif//WITH_MPI

#endif//ESL_PRICE_HPP
