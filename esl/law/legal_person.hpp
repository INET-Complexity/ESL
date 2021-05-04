/// \file   legal_person.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-03-23
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
#ifndef ESL_LEGAL_PERSON_HPP
#define ESL_LEGAL_PERSON_HPP

#include <variant>

#include <boost/serialization/serialization.hpp>

#include <esl/agent.hpp>
#include <esl/economics/owner.hpp>

#include <esl/geography/countries.hpp>
#include <esl/law/jurisdiction.hpp>

#include <esl/law/government.hpp>
#include <esl/law/legal_entity.hpp>
#include <esl/law/natural_person.hpp>


namespace esl::law {

    ///
    /// The reason this is a separate class is so we can implement a parser that
    /// automatically distinguishes between possible codes and systems (where
    /// possible)
    ///
    struct legal_person
    : public virtual agent
    , public virtual owner<property>
    {
        std::variant<legal_entity, natural_person, government> representation;

        jurisdiction primary_jurisdiction;

    private:
        friend class boost::serialization::access;

        legal_person()
        : legal_person(identity<agent>(),
                       jurisdiction(esl::geography::countries::US,
                                    esl::economics::currencies::USD))
        {}

    public:
        explicit legal_person(const identity<agent> &i,
                              esl::law::jurisdiction primary_jurisdiction)
        : agent(i)
        , representation(legal_entity())
        , primary_jurisdiction(primary_jurisdiction)
        {}

        legal_person(const identity<agent> &i,
                     const legal_entity &le,
                     esl::law::jurisdiction primary_jurisdiction)
        : agent(i)
        , representation(le)
        , primary_jurisdiction(primary_jurisdiction)
        {}

        legal_person(const identity<agent> &i,
                     const natural_person &np,
                     esl::law::jurisdiction primary_jurisdiction)
        : agent(i)
        , representation(np)
        , primary_jurisdiction(primary_jurisdiction)
        {}

        legal_person(const identity<agent> &i,
                     const government &g,
                     esl::law::jurisdiction primary_jurisdiction)
        : agent(i)
        , representation(g)
        , primary_jurisdiction(primary_jurisdiction)
        {}

        template<class archive_t>
        void save(archive_t &archive, const unsigned int version) const
        {
            (void)version;
            size_t index_ = representation.index();
            archive << boost::serialization::make_nvp("variant", index_);
            switch(index_) {
            case 0:
                archive << boost::serialization::make_nvp(
                    "legal_entity", std::get<legal_entity>(representation));
                break;
            case 1:
                archive << boost::serialization::make_nvp(
                    "natural_person", std::get<natural_person>(representation));
                break;
            case 2:
                archive << boost::serialization::make_nvp(
                    "government", std::get<government>(representation));
                break;
            }
        }

        template<class archive_t>
        void load(archive_t &archive, const unsigned int version)
        {
            (void)version;
            size_t index_ = 0;
            archive >> boost::serialization::make_nvp("variant", index_);
            if(0 == index_) {
                legal_entity le;
                archive >> boost::serialization::make_nvp("legal_entity", le);
                representation.emplace<0>(le);
            } else if(1 == index_) {
                natural_person np;
                archive >> boost::serialization::make_nvp("natural_person", np);
                representation.emplace<1>(np);
            } else if(2 == index_) {
                government g;
                archive >> boost::serialization::make_nvp("government", g);
                representation.emplace<2>(g);
            }
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(agent);
            split_member(archive, *this, version);
            archive &BOOST_SERIALIZATION_NVP(primary_jurisdiction);
        }
    };
}  // namespace esl::law

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::law::legal_person> : mpl::false_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_LEGAL_PERSON_HPP
