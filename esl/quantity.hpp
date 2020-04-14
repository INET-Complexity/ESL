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
        /// \note   We explicitly delete the division into quantity operator as
        /// this
        ///         loses the remainder. Instead, use
        ///         quantity::divide()
        template<typename divisor_type_>
        quantity &operator/=(divisor_type_) const = delete;

        ///
        ///`\brief  The number of parts
        ///
        std::uint64_t amount;

        ///
        /// \brief  The number of sub-units to make one part
        ///
        std::uint64_t basis;

        ///
        /// \param amount   The number of parts of something, e.g. 23 cents.
        ///                 Default: 0
        /// \param basis    The number of parts to make one unit, e.g.
        ///                 100 cents to 1 dollar. Default: 1
        explicit constexpr quantity(std::uint64_t amount = 0,
                                    std::uint64_t basis  = 1)
        : amount(amount), basis(basis)
        {
            if(basis < 1) {
                throw std::logic_error("quantity with 0 basis");
            }
        }

        ///
        /// \param q
        constexpr quantity(const quantity &q)
        : quantity(q.amount, q.basis)
        {

        }

        ///
        /// \param operand
        /// \return
        constexpr void assert_equal_basis(const quantity &operand) const
        {
            if(basis != operand.basis) {
                throw std::logic_error("assert_equal_basis");
            }
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
                iterator_->basis  = basis;
            }

            for(; iterator_ != result_.end(); ++iterator_) {
                iterator_->amount = quotient_;
                iterator_->basis  = basis;
            }

            return result_;
        }


        [[nodiscard]] constexpr quantity
        operator*(const std::uint64_t &operand) const
        {
            return quantity(amount * operand, basis);
        }

        [[nodiscard]]
#if !EXCEPTION_ON_BASIS_CHANGE
        constexpr
#endif
        quantity
        operator*(const quantity &operand) const
        {
            auto numerator_ = amount * operand.amount;
            auto remainder_ = numerator_ % operand.basis;

            if(!remainder_) {
                return quantity(numerator_ / operand.basis, basis);
            }

            if(EXCEPTION_ON_BASIS_CHANGE) {
                throw std::logic_error("EXCEPTION_ON_BASIS_CHANGE");
            }

            auto coremainder_ = numerator_ % basis;

            if(!coremainder_) {
                return quantity(numerator_ / basis, operand.basis);
            }

            return quantity(numerator_, basis * operand.basis);
        }

        ///
        /// \param operand
        /// \return
        quantity &operator*=(const std::uint64_t &operand)
        {
            (*this) = (*this) * operand;
            return *this;
        }

        ///
        /// \brief  Add together two quantities.
        ///
        /// \param operand
        /// \return
        [[nodiscard]]
#if !EXCEPTION_ON_BASIS_CHANGE
        constexpr
#endif
        quantity operator+(const quantity &operand) const
        {
            auto numerator_ = amount * operand.basis + operand.amount * basis;
            auto remainder_ = numerator_ % operand.basis;

            if(!remainder_) {
                return quantity(numerator_ / operand.basis, basis);
            }

            if(EXCEPTION_ON_BASIS_CHANGE) {
                throw std::logic_error("EXCEPTION_ON_BASIS_CHANGE");
            }

            auto coremainder_ = numerator_ % basis;

            if(!coremainder_) {
                return quantity(numerator_ / basis, operand.basis);
            }

            return quantity(numerator_, basis * operand.basis);
        }

        ///
        /// \param operand
        /// \return
        quantity &operator+=(const quantity &operand)
        {
            (*this) = (*this) + operand;
            return *this;
        }

        ///
        /// \brief  subtract two quantities.
        ///
        /// \param operand
        /// \return
        [[nodiscard]]
#if !EXCEPTION_ON_BASIS_CHANGE
        constexpr
#endif
        quantity operator-(const quantity &operand) const
        {
            auto left_  = amount * operand.basis;
            auto right_ = operand.amount * basis;
            if(right_ > left_) {
                throw std::logic_error(
                    "subtraction results in negative quantity");
            }
            auto numerator_ = left_ - right_;
            auto remainder_ = numerator_ % operand.basis;

            if(!remainder_) {
                return quantity(numerator_ / operand.basis, basis);
            }

            if(EXCEPTION_ON_BASIS_CHANGE) {
                throw std::logic_error("EXCEPTION_ON_BASIS_CHANGE");
            }

            auto coremainder_ = numerator_ % basis;

            if(!coremainder_) {
                return quantity(numerator_ / basis, operand.basis);
            }

            return quantity(numerator_, basis * operand.basis);
        }


        quantity &operator=(const quantity &operand)
        {
            amount = operand.amount;
            basis  = operand.basis;
            return *this;
        }

        ///
        /// \param operand
        /// \return
        quantity &operator-=(const quantity &operand)
        {
            (*this) = (*this) - operand;
            return *this;
        }

        ///
        /// \brief  true if equal to the truncated double equivalent of this
        ///         quantity
        ///
        /// \param operand
        /// \return
        [[nodiscard]] constexpr bool operator==(double operand) const
        {
            return (amount == std::uint64_t(operand * basis));
        }

        ///
        /// \param operand
        /// \return
        [[nodiscard]] constexpr bool operator==(const quantity &operand) const
        {
            return (amount == operand.amount && basis == operand.basis);
        }

        [[nodiscard]] constexpr bool operator!=(double operand) const
        {
            return !((*this) == operand);
        }

        [[nodiscard]] constexpr bool operator<(double operand) const
        {
            return (amount < operand * basis);
        }

        [[nodiscard]] constexpr bool operator<(const quantity &operand) const
        {
            return (amount * basis < operand.amount * operand.basis);
        }

        [[nodiscard]] constexpr bool operator>(double operand) const
        {
            return (amount > operand * basis);
        }

        [[nodiscard]] constexpr bool operator>(const quantity &operand) const
        {
            return (amount * basis > operand.amount * operand.basis);
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
        operator/(std::uint64_t divisor) const
        {
            std::uint64_t quotient_  = amount / divisor;
            std::uint64_t remainder_ = amount % divisor;

            if(divisor >= (2 * remainder_)) {
                std::vector<quantity> result_(divisor,
                                              quantity(quotient_, basis));
                for(auto iterator_ = result_.begin();
                    iterator_ != result_.begin() + remainder_;
                    ++iterator_) {
                    iterator_->amount = quotient_ + 1;
                }
                return result_;
            }

            std::vector<quantity> result_(divisor,
                                          quantity(quotient_ + 1, basis));
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
            return double(amount) / basis;
        }

        friend std::ostream &operator<<(std::ostream &stream, const quantity &q)
        {
            stream << q.amount;
            if(q.basis != 1) {
                stream << '/';
                stream << q.basis;
            }
            return stream;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_NVP(amount);
            archive &BOOST_SERIALIZATION_NVP(basis);
        }
    };

}  // namespace esl


#endif  // ESL_AMOUNT_HPP
