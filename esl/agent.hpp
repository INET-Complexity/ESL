/// \file   agent.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2017-11-24
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
#ifndef ESL_AGENT_HPP
#define ESL_AGENT_HPP

#include <utility>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <esl/data/producer.hpp>
#include <esl/interaction/communicator.hpp>
#include <esl/simulation/entity.hpp>
#include <esl/simulation/identity.hpp>


namespace esl {
    class agent
    : public entity<agent>
    , public interaction::communicator
    , public data::producer
    {
    protected:
    private:
        friend class boost::serialization::access;

    public:
        ///
        /// \brief  agent default constructor
        ///
        agent();

        ///
        /// \brief
        /// \param i    identity for the agent
        explicit agent(esl::identity<agent> i);

        ///
        /// \brief  Copy constructor
        ///
        /// \param o    other agent to copy
        agent(const agent &o);

        virtual ~agent() = default;

        ///
        /// \param step The left-closed time interval [lower, upper), where
        ///             lower denotes the current time.
        ///
        /// \return The time at which the agent expects the next event, where
        ///         it is expected that for the return value v:
        ///              lower <= v < +infinity
        ///
        virtual simulation::time_point act(simulation::time_interval step,
                                           std::seed_seq &seed);

        ///
        /// \param rhs
        /// \return
        [[nodiscard]] constexpr bool operator != (const agent &rhs) const
        {
            return !(*this == rhs);
        }

        ///
        /// \return friendly description of the agent, which should mention
        ///         the identifier somewhere
        ///
        [[nodiscard]] virtual std::string describe() const
        {
            std::stringstream stream_;
            stream_ << "agent " << identifier;
            return stream_.str();
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(entity<agent>);
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(
                interaction::communicator);
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(data::producer);
        }
    };

}  // namespace esl


namespace std {
    ///
    /// \brief  Hash function for agent class redirects to
    ///         \ref std::hash<entity<agent>>
    ///
    template<>
    struct hash<esl::agent>
    {
        size_t operator()(const esl::agent &a) const
        {
            return std::hash<esl::entity<esl::agent>>()(a);
        }
    };
}  // namespace std


#ifdef WITH_MPI

#include <boost/mpi.hpp>
namespace boost ::mpi {
    template<>
    struct is_mpi_datatype<esl::agent> : public mpl::false_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_AGENT_HPP