/// \file   company.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-27
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
#ifndef ESL_COMPANY_HPP
#define ESL_COMPANY_HPP

#include <iostream>
#include <map>
#include <numeric>
#include <vector>

#include <boost/serialization/serialization.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>


#include <esl/economics/cash.hpp>
#include <esl/economics/finance/dividend.hpp>
#include <esl/economics/finance/shareholder.hpp>
#include <esl/economics/finance/bondholder.hpp>
#include <esl/law/organization.hpp>
#include <esl/economics/accounting/balance_sheet.hpp>
#include <esl/economics/accounting/cash_flow_statement.hpp>


namespace esl::economics {
    ///
    /// \brief
    ///
    struct company
    : public law::organization
    , public finance::shareholder
    , public finance::bondholder
    , public identifiable_as<company>
    {
    public:
        accounting::balance_sheet balance_sheet;

        ///
        /// \brief  The number of shares outstanding for each share class
        ///
        std::map<finance::share, std::uint64_t> shares_outstanding;

        ///
        /// \brief Stores shareholders for each share class
        ///
        std::unordered_map<identity<shareholder>,
                           std::map<finance::share, std::uint64_t>>
            shareholders;

        ///
        /// \brief  Gets the set of shareholders for all share classes
        ///
        std::set<identity<shareholder>> unique_shareholders() const;

        ///
        /// \brief  Computes outstanding shares across all share classes
        ///
        /// \return
        std::uint64_t total_shares() const;

        ///
        /// \warning: TODO advanced share rules including preference, cumulative
        /// shares etc \warning: the rounding rule is in favor of the company,
        /// fractional currency dividends are not paid out! \param
        /// unappropriated_profit \return
        std::map<finance::share, std::tuple<std::uint64_t, price>>
        compute_dividend_per_share(const price &unappropriated_profit);

    protected:
        simulation::time_point last_announced_;

        simulation::time_point last_payment_;

        friend class boost::serialization::access;

        company();

    public:
        explicit company(const identity<company> &i, const law::jurisdiction &j);

        ///
        /// \brief  Returns the next dividend payment date, or none if no
        ///         dividend payment is scheduled.
        ///         The detailed behaviour is to not pay out dividends to
        ///         shareholders.
        ///
        /// \param interval
        /// \return
        virtual std::optional<finance::dividend_policy>
        upcoming_dividend(simulation::time_interval interval, std::seed_seq &seed);

        ///
        /// \brief  the default company.act pays out all unappropriated profits
        ///         to shareholders at the end of the currently specified
        ///         interval.
        ///
        /// \note   Other default behaviours lead to the company reinvesting
        ///         profits if there are no shareholders, and making no payments
        ///         if there are no unappropriated profits or even losses.
        ///
        /// \param interval
        /// \return
        simulation::time_point act(simulation::time_interval interval,
                                   std::seed_seq &) override;

        friend inline std::ostream &operator<<(std::ostream &o, const company &c)
        {
            o << c.identifier;
            return o;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(organization);
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(shareholder);
            archive &boost::serialization::make_nvp(
                "identifiable_as〈company〉",
                boost::serialization::base_object<identifiable_as<company>>(
                    *this));

            archive &BOOST_SERIALIZATION_NVP(shares_outstanding);
            archive &BOOST_SERIALIZATION_NVP(shareholders);
        }
    };
}  // namespace esl::economics

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::economics::company>
    : public mpl::false_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_COMPANY_HPP
