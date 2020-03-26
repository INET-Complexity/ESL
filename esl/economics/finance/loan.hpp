/// \file   loan.hpp
///
/// \brief  Base class for loans, wherein funds are given to a borrower, who
///         then through a series of payments pays back these funds.
///         For lending economic assets, see esl::economics::finance::lease.
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-29
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
#ifndef ESL_LOAN_HPP
#define ESL_LOAN_HPP

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>

#include <esl/agent.hpp>
#include <esl/law/contract.hpp>


namespace esl::economics::finance {

    ///
    ///
    ///
    struct loan
    : esl::law::contract
    {
        [[nodiscard]] const esl::identity<esl::agent> &lender() const
        {
            return parties[0];
        }

        [[nodiscard]] const esl::identity<esl::agent> &borrower() const
        {
            return parties[1];
        }

        explicit loan( esl::identity<esl::agent> lender
                     , esl::identity<esl::agent> borrower
                     )
        : esl::law::contract({lender, borrower})
        {

        }

        virtual ~loan() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &boost::serialization::make_nvp("contract"
                , boost::serialization::base_object<contract>(*this));

        }
    };

}  // namespace esl::economics::finance

///
/// \brief  Hash definition for loan
///
namespace std {
    template<>
    struct hash<esl::economics::finance::loan>
    {
        std::size_t operator()(const esl::economics::finance::loan &c) const
        {
            return std::hash<esl::entity<esl::law::property>>()(c);
        }
    };
}  // namespace std


#endif  // ESL_LOAN_HPP
