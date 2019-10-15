/// \file   shareholder.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-03-29
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
#ifndef ESL_SHAREHOLDER_HPP
#define ESL_SHAREHOLDER_HPP

#include <map>
#include <tuple>
#include <unordered_map>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>

#include <esl/economics/owner.hpp>
#include <esl/economics/cash.hpp>
#include <esl/economics/finance/stock.hpp>



namespace esl::economics {
    struct company;
}


namespace esl::economics::finance {

    ///
    /// \brief  An interface that adds shareholder functionality to agents.
    ///
    /// \note   A shareholder automatically filters transfers for cash and stock
    ///         and therefore inherits law::owner<cash> and law::owner<stock>
    ///
    struct shareholder
    : public virtual law::owner<cash>
    , public virtual law::owner<stock>
    , public identifiable_as<shareholder>
    {
    public:
        shareholder();

    public:
        ///
        /// A datastructure relating the shares of a company to stocks
        ///
        std::map<std::tuple<identity<company>, share>, identity<law::property>>
            stocks;

        ///
        /// \brief  The valuation of
        ///
        std::map<identity<law::property>, price> prices;


        ///
        /// \brief Datastructure to map share holdings to the issuing company
        ///
        typedef std::unordered_map<identity<company>,
                                   std::map<share, std::uint64_t>>
            share_holdings;

        share_holdings shares;

        ///
        /// \return
        std::map<identity<law::property>, quantity> stock_holdings() const;

        ///
        /// TODO: other mapping, perhaps including identity<property>?
        ///
        std::set<std::tuple<simulation::time_point, identity<company>>>
            ex_dividend_dates;

        explicit shareholder(const identity<shareholder> &i);

        virtual ~shareholder()
        {

        }

        simulation::time_point
        submit_dividend_record(simulation::time_interval interval);

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_NVP(shares);
            archive &BOOST_SERIALIZATION_NVP(ex_dividend_dates);
        }
    };
}  // namespace esl::economics::finance

#endif  // ESL_SHAREHOLDER_HPP
