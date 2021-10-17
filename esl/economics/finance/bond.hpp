/// \file   bond.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-02-03
/// \copyright  Copyright 2017-2020 The Institute for New Economic Thinking,
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
#ifndef ME_BOND_HPP
#define ME_BOND_HPP

#include <variant>

#include <esl/economics/finance/security.hpp>
#include <esl/economics/interest_rate.hpp>
#include <esl/law/legal_person.hpp>

#ifdef WITH_QL
#include <ql/qldefines.hpp>
#ifdef BOOST_MSVC
#include <ql/auto_link.hpp>
#endif
#include <ql/cashflows/couponpricer.hpp>
#include <ql/indexes/ibor/euribor.hpp>
#include <ql/indexes/ibor/usdlibor.hpp>
#include <ql/instruments/bonds/floatingratebond.hpp>
#include <ql/instruments/bonds/zerocouponbond.hpp>
#include <ql/pricingengines/bond/discountingbondengine.hpp>
#include <ql/termstructures/volatility/optionlet/constantoptionletvol.hpp>
#include <ql/termstructures/yield/bondhelpers.hpp>
#include <ql/termstructures/yield/piecewiseyieldcurve.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/time/daycounters/one.hpp>
#include <ql/time/daycounters/thirty360.hpp>

#endif



namespace esl::economics::finance {
    struct bond
    : public virtual security
    , public identifiable_as<bond>
    {
        struct coupon_base
        {

        };

        struct fixed_coupon
        : coupon_base
        {
            nominal_interest_rate rate;
            fixed_coupon(nominal_interest_rate rate = nominal_interest_rate(0))
            : rate(rate)
            {

            }
        };


        // std::variant<nominal_interest_rate/*, indexed_interest_rate*/>
        // coupon;
        std::shared_ptr<coupon_base> coupon;

        esl::simulation::time_point effective;

        esl::simulation::time_point maturity;


        bond(esl::identity<bond> i, esl::simulation::time_point maturity,
             std::shared_ptr<coupon_base> coupon, const law::legal_person &issuer )
        : property(i), security(i), coupon(coupon), maturity(maturity)
        {

        }

        virtual ~bond() = default;

        std::string name() const override
        {
            std::stringstream stream_;
            stream_ << "bond" << ' ' << this->identifier;
            return stream_.str();
        }
    };

    struct convertible_bond : public bond
    {};
}  // namespace esl::economics::finance

#endif  // ME_BOND_HPP
