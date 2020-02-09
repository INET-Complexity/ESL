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

#include <esl/economics/interest_rate.hpp>
#include <esl/economics/finance/security.hpp>


namespace esl::economics::finance {
    struct bond
    : public virtual security
    , public identifiable_as<bond>
    {
        std::variant<nominal_interest_rate/*, indexed_interest_rate*/> coupon;

        esl::simulation::time_point maturity;

        /*
        bond( esl::identity<bond> i
                , esl::simulation::time_point maturity
            , nominal_interest_rate coupon = nominal_interest_rate(0, esl::simulation::time_duration(252) ))
        : property(i)
        , security(i, )
        , coupon(coupon)
        , maturity(maturity)
        {

        }*/
    };

    struct convertible_bond
    : public bond
    {

    };
}

#endif //ME_BOND_HPP
