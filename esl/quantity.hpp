/// \file   quantity.hpp
///
/// \brief  A class used to enforce conversion without numerical error when
///         counting property, goods and other items.
///
/// \authors    Maarten P. Scholl
/// \date       2018-02-05
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
#ifndef ESL_AMOUNT_HPP
#define ESL_AMOUNT_HPP

#include <array>
#include <cstdint>
#include <vector>

#include <iostream>

#include "exception.hpp"
#include <boost/serialization/nvp.hpp>

#ifndef EXCEPTION_ON_BASIS_CHANGE
#define EXCEPTION_ON_BASIS_CHANGE true
#endif


namespace esl {
    ///
    /// \brief  The quantity class is used to losslessly deal with quantities of
    ///         economic goods in large quantities. A quantity is associated
    ///         with a basis (the number of parts that make up a unit amount of
    ///         the object).
    ///
    /// \details    The quantity is not a fraction: a property that dividies
    ///             into single parts, is not the same as a property that
    ///             divides into 100 parts. Hence, quantity(1,1) is not the same
    ///             as quantity(100,100). Implicit conversion or combination of
    ///             `quantity` with different `basis` is not encouraged, and the
    ///             preprocessor option EXCEPTION_ON_BASIS_CHANGE can be used to
    ///             disallow this.
    ///
    struct quantity
    {
    private:
    public:
        ///
        /// \brief  The
        ///
        std::uint64_t amount;

        ///
        /// \param amount   The number of parts of something, e.g. 23 cents.
        ///                 Default: 0
        constexpr quantity(std::uint64_t amount = 0)
        : amount(amount)
        {

        }

        ///
        /// \param q
        constexpr quantity(const quantity &q)
        : quantity(q.amount)
        {

        }

        ///
        /// \tparam divisor_
        /// \return
        template<uint64_t divisor_>
        constexpr std::array<quantity, divisor_> divide() const
        {
            static_assert(divisor_ != 0, "division by zero");

            std::uint64_t quotient_  = amount / divisor_;
            std::uint64_t remainder_ = amount % divisor_;

            std::array<quantity, divisor_> result_;

            auto iterator_ = result_.begin();
            for(; iterator_ != result_.begin() + remainder_; ++iterator_) {
                iterator_->amount = quotient_ + 1;
            }

            for(; iterator_ != result_.end(); ++iterator_) {
                iterator_->amount = quotient_;
            }

            return result_;
        }

        ///
        /// \brief  Integer division which loses the remainder.
        ///         See quantity::divide to recover the remainder
        ///
        template<typename divisor_type_>
        quantity &operator /= (divisor_type_ value) const
        {
            amount /= value;
        }


        [[nodiscard]] constexpr quantity
        operator * (const std::uint64_t &operand) const
        {
            return quantity(amount * operand);
        }

        [[nodiscard]] constexpr quantity
        operator * (const quantity &operand) const
        {
            return quantity(amount * operand.amount);
        }

        ///
        /// \param operand
        /// \return
        quantity &operator *= (const std::uint64_t &operand)
        {
            (*this) = (*this) * operand;
            return *this;
        }

        ///
        /// \brief  Add together two quantities.
        ///
        /// \param operand
        /// \return
        [[nodiscard]] constexpr
        quantity operator + (const quantity &operand) const
        {
            return quantity(amount + operand.amount);
        }

        ///
        /// \param operand
        /// \return
        quantity &operator += (const quantity &operand)
        {
            (*this) = (*this) + operand;
            return *this;
        }

        ///
        /// \brief  subtract two quantities.
        ///
        /// \param operand
        /// \return
        [[nodiscard]] constexpr quantity operator - (const quantity &operand) const
        {
            if(amount < operand.amount) {
                throw esl::exception("subtraction results in negative quantity");
            }
            return quantity(amount - operand.amount);
        }


        quantity &operator = (const quantity &operand)
        {
            amount = operand.amount;
            return *this;
        }

        ///
        /// \param operand
        ///
        quantity &operator -= (const quantity &operand)
        {
            (*this) = (*this) - operand;
            return *this;
        }

        ///
        /// \brief
        ///
        /// \param operand
        /// \return
        [[nodiscard]] constexpr bool operator == (double operand) const
        {
            return (amount == operand);
        }

        ///
        /// \param operand
        /// \return
        [[nodiscard]] constexpr bool operator == (const quantity &operand) const
        {
            return (amount == operand.amount);
        }

        [[nodiscard]] constexpr bool operator!=(double operand) const
        {
            return !((*this) == operand);
        }

        [[nodiscard]] constexpr bool operator < (double operand) const
        {
            return (amount < operand);
        }

        [[nodiscard]] constexpr bool operator < (const quantity &operand) const
        {
            return amount < operand.amount;
        }

        [[nodiscard]] constexpr bool operator > (double operand) const
        {
            return amount > operand;
        }

        [[nodiscard]] constexpr bool operator > (const quantity &operand) const
        {
            return amount > operand.amount;
        }

        [[nodiscard]] constexpr bool operator<=(double operand) const
        {
            return (*this) < operand || (*this) == operand;
        }

        [[nodiscard]] constexpr bool operator<=(const quantity &operand) const
        {
            return (*this) < operand || (*this) == operand;
        }

        [[nodiscard]] constexpr bool operator>=(double operand) const
        {
            return (*this) > operand || (*this) == operand;
        }

        [[nodiscard]] constexpr bool operator>=(const quantity & operand) const
        {
            return (*this) > operand || (*this) == operand;
        }

        ///
        /// \brief  Division into `divisor` parts, allocating the remainder
        /// starting at the first quotient.
        ///
        /// \param divisor
        /// \return
        [[nodiscard]] std::vector<quantity>
        operator / (std::uint64_t divisor) const
        {
            std::uint64_t quotient_  = amount / divisor;
            std::uint64_t remainder_ = amount % divisor;

            if(divisor >= (2 * remainder_)) {
                std::vector<quantity> result_(divisor, quantity(quotient_));
                for(auto iterator_ = result_.begin();
                    iterator_ != result_.begin() + remainder_;
                    ++iterator_) {
                    iterator_->amount = quotient_ + 1;
                }
                return result_;
            }

            std::vector<quantity> result_(divisor, quantity(quotient_ + 1));
            for(auto iterator_ = result_.begin() + remainder_;
                iterator_ != result_.end();
                ++iterator_) {
                iterator_->amount = quotient_;
            }
            return result_;
        }

        ///
        /// \return amount/basis as a double precision floating point number.
        ///
        [[nodiscard]] explicit operator double() const
        {
            return double(amount);
        }

        friend std::ostream &operator<<(std::ostream &stream, const quantity &q)
        {
            stream << q.amount;
            return stream;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_NVP(amount);
        }
    };

}  // namespace esl


#endif  // ESL_AMOUNT_HPP
