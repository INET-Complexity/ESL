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

#include <esl/interaction/communicator.hpp>
#include <esl/simulation/entity.hpp>
#include <esl/data/producer.hpp>

namespace esl {

    class agent
    : public entity<agent>
    , public interaction::communicator
    , public data::producer
    {
    protected:
    private:
        friend class boost::serialization::access;

        agent();

    public:
        explicit agent(identity<agent> i);

        ///
        /// \brief  Copy constructor
        ///
        /// \param o
        agent(const agent &o);

        virtual ~agent() = default;

        ///
        /// \param start
        /// \return
        virtual simulation::time_point act(simulation::time_interval step, std::seed_seq &seed);

        ///
        /// \param rhs
        /// \return
        [[nodiscard]] constexpr bool operator==(const agent &rhs) const
        {
            return identifier == rhs.identifier;
        }

        ///
        /// \param rhs
        /// \return
        [[nodiscard]] constexpr bool operator!=(const agent &rhs) const
        {
            return !(*this == rhs);
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;

            archive &boost::serialization::make_nvp(
                "entity⟨agent⟩",
                boost::serialization::base_object<entity<agent>>(*this));
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(communicator);
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
    struct is_mpi_datatype<esl::agent>
    : public mpl::false_
    {

    };
}
#endif//WITH_MPI

#endif  // ESL_AGENT_HPP