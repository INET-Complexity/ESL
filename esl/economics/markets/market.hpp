/// \file   market.hpp
///
/// \brief  Abstract base for markets
///
/// \authors    Maarten P. Scholl
/// \date       2018-05-01
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
#ifndef ESL_MARKET_HPP
#define ESL_MARKET_HPP

#include <boost/serialization/nvp.hpp>

#include <esl/agent.hpp>
#include <esl/economics/owner.hpp>
#include <esl/economics/markets/quote.hpp>
#include <esl/economics/markets/ticker.hpp>


namespace esl::economics {
    ///
    /// \brief  A market intermediates between market participants (formally,
    /// property holders).
    ///         This abstract class only represents the abstract intermediation
    ///         (communication) part - in a concrete model we expect the user to
    ///         unite the market part together with perhaps a legal entity or
    ///
    struct market
    : public virtual agent
    , public identifiable_as<market>
    {
    public:
        ///
        /// \brief  The set of assets traded in this market.
        ///
        std::vector<esl::economics::markets::ticker> traded_properties;

        ///
        /// \brief  The list of market participants that the market communicates
        /// with. No other agents can interact with the
        ///         market.
        ///
        std::unordered_set<identity<law::owner<law::property>>> participants;

        ///
        /// \param  execution, the execution date of the transaction in the market
        /// \return the time_point on which the transacted assets are
        ///             received
        ///
        virtual simulation::time_point
        settlement(const simulation::time_point &execution) const
        {
            return execution;
        }

    protected:
        friend class boost::serialization::access;

        ///
        /// \brief  default constructor is restricted, because we need the
        ///         identifier and ticker collection
        ///
        market() 
        : market(identity<agent>())
        {

        }

    public:
        ///
        /// \brief  Construct a market with a collection of tickers
        ///
        explicit market(identity<agent> i,
                        std::vector<markets::ticker> traded = {})
        : agent(i)
        , traded_properties(move(traded))
        { 
        
        }

        virtual ~market() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(agent);
            archive &BOOST_SERIALIZATION_NVP(traded_properties);
            archive &BOOST_SERIALIZATION_NVP(participants);
        }
    };
}  // namespace esl::economics

namespace std {
    ///
    /// \brief  Hash definition for market class. The implementation uses the
    ///         market-agent identifier.
    ///
    template<>
    struct hash<esl::economics::market>
    {
        ///
        /// \param m    The market to compute a hash for.
        /// \return     The hash is based on entity<agent>, using the identifier
        size_t operator()(const esl::economics::market &m) const
        {
            return std::hash<esl::entity<esl::agent>>()(m);
        }
    };
}  // namespace std

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::economics::market>
    : public mpl::false_
    {

    };
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_MARKET_HPP
