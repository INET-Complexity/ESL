/// \file   bondholder.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-01-03
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
#ifndef ESL_BONDHOLDER_HPP
#define ESL_BONDHOLDER_HPP

#include <map>
#include <tuple>
#include <unordered_map>

//#include <boost/serialization/nvp.hpp>
//#include <boost/serialization/serialization.hpp>

#include <esl/economics/owner.hpp>
#include <esl/economics/cash.hpp>
#include <esl/economics/finance/bond.hpp>



namespace esl::economics {
    struct company;
}


namespace esl::economics::finance {
    ///
    /// \brief  An interface that adds bondholder functionality to agents.
    ///
    /// \note   A shareholder automatically filters transfers for cash and stock
    ///         and therefore inherits law::owner<cash> and law::owner<stock>
    ///
    struct bondholder
    : public virtual law::owner<cash>
    , public virtual law::owner<bond>
    , public identifiable_as<bondholder>
    {
    public:
        bondholder();

    public:



        ///
        /// \brief  The valuation of the stocks
        ///
        esl::law::property_map<price> bond_prices;

        esl::law::property_map<nominal_interest_rate> yields;


        explicit bondholder(const identity<bondholder> &i);

        virtual ~bondholder() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
        }
    };
}  // namespace esl::economics::finance

#endif  // ESL_BONDHOLDER_HPP
