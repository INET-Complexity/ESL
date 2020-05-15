/// \file   standard.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-01
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
#ifndef ESL_STANDARD_HPP
#define ESL_STANDARD_HPP

#include <esl/economics/iso_4217.hpp>

#include <esl/data/log.hpp>
#include <esl/economics/finance/securities_lending_contract.hpp>
#include <esl/economics/finance/stock.hpp>
#include <esl/economics/markets/quote.hpp>
#include <esl/economics/money.hpp>
#include <esl/economics/cash.hpp>


namespace esl::economics::accounting {

    ///
    /// \brief  The accounting method wraps all parameters to value assets. For
    ///         example, model parameters for mark-to-model valuation, and
    ///         market data or calibrated structures for mark-to-market
    ///         valuation.
    ///
    /// \details    The default accounting standard will value assets and
    ///             liabilities in their domestic currency, and then convert
    ///             these values to the reporting currency where conversion
    ///             rates are provided
    ///
    ///
    struct standard
    {
        const iso_4217 reporting_currency;

        ///
        /// \brief  A mapping from foreign currencies to the reporting_currency
        ///
        std::map<iso_4217, exchange_rate> foreign_currencies;

        ///
        /// \brief  A set of properties that are to be marked to market.
        ///
        std::map<identity<law::property>, quote> mark_to_market;

        explicit standard(iso_4217 reporting_currency);

        [[nodiscard]] price value(const money &m, const quantity &q) const
        {
            auto foreign_ = price(int64_t(q.amount), m.denomination);
            if(m.denomination == reporting_currency)
            {
                return foreign_;
            }
            auto i = foreign_currencies.find(m.denomination);
            if(i == foreign_currencies.end()){
                std::stringstream message_;
                message_ << "no exchange rate provided converting ";
                message_ << m.denomination << " to " << reporting_currency;
                LOG(error) << message_.str() << std::endl;
                throw std::domain_error(message_.str().c_str());
            }
            return i->second * foreign_;
        }

        [[nodiscard]] price value(const esl::economics::cash &c, const quantity &q) const
        {
            return value((const money &)c, q);
        }

        [[nodiscard]] price value(const finance::stock &s, const quantity &q) const
        {
            auto i = mark_to_market.find(s.identifier);
            if(mark_to_market.end() == i){
                LOG(error) << "no market price for stock " << s.identifier << std::endl;
                throw std::logic_error("no market price");
            }

            int64_t value_ = (std::get<price>(i->second.type).value * q.amount)
                / (q.basis * i->second.lot);

            return price(value_, std::get<price>(i->second.type).valuation);
        }

        [[nodiscard]] price value(const finance::loan &l, const quantity &q) const
        {
            (void) l;
            return price(-1.00 * double(q), reporting_currency);
        }

        [[nodiscard]] price value(const finance::securities_lending_contract &c, const quantity &q) const
        {
            price result_ = price(0.00, reporting_currency);
            auto i = mark_to_market.find(c.security);
            if(mark_to_market.end() == i){
                LOG(error) << "no market price for stock " << c.security << std::endl;
                throw std::logic_error("no market price");
            }
            // TODO: use foreign_currencies if valuation is in different currency
            int64_t value_ = (std::get<price>(i->second.type).value * c.size.amount * q.amount)
                             / (q.basis * i->second.lot);
            result_.value -= value_;
            //LOG(trace) << "value of " << q << " short contracts = " << result_ << std::endl;
            return result_;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_NVP(reporting_currency);
        }
    };
}

#endif  // ESL_STANDARD_HPP
