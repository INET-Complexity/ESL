/// \file   entity.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-01-19
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
/// Oxford Martin School, University of Oxford
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
#ifndef ESL_SIMULATION_ENTITY_HPP
#define ESL_SIMULATION_ENTITY_HPP

#include <iostream>
#include <type_traits>
#include <utility>
using std::move;

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
using boost::serialization::make_nvp;

#include <boost/serialization/strong_typedef.hpp>

#include <iostream>
using std::ostream;

#include <vector>
using std::vector;


//#include <esl/simulation/identity.hpp>
#include <esl/simulation/identifiable_as.hpp>
//#include <esl/simulation/output.hpp>


namespace esl {
    template<typename entity_type_>
    struct entity;

    ///
    /// \brief  This specialization is reserved to be the root of every entity.
    /// \example    It is used to test whether some type T is an entity, e.g.
    ///             is_base_of<entity<void>, T>
    ///
    template<>
    struct entity<void>
    {
    protected:
        entity()          = default;
        virtual ~entity() = default;
    };

    ///
    /// \brief  An entity is an object that is distinguishable from other
    /// objects, meaning we can tell it apart from
    ///         value objects (fungible or interchangeable) and other entity
    ///         objects
    ///
    /// \tparam entity_type_ the type that is to be distinguished between
    ///
    template<typename entity_type_>
    struct entity
    : entity<void>
    , public identifiable_as<entity_type_>
    {
    public:
        /// \typedef    entity_type captures the template typename parameter
        /// @entity_type_ so that the dependent type can later be looked up
        //  \example    entity<agent>::entity_type a; // a is of type 'agent'
        // typedef entity_type_ entity_type;

        friend class boost::serialization::access;
        ///
        /// \brief  The identifying code for this entity.
        ///
        const identity<entity_type_> identifier;

    private:
        friend class identity<entity_type_>;
        ///
        /// \brief  The number of children this entity has created
        ///
        typename identity<entity_type_>::digit_t children_;

        ///
        ///
        ///
        entity() : entity(identity<entity_type_>())
        {}

    public:
        ///
        ///
        /// \param identifier
        explicit entity(identity<entity_type_> identifier)
        : identifier(move(identifier)), children_(0)
        {}

        template<typename child_t_>
        identity<child_t_> create()
        {
            auto prefix_ = vector<uint64_t>(this->identifier.digits);
            prefix_.push_back(children_);
            ++children_;
            prefix_.shrink_to_fit();
            return identity<child_t_>(move(prefix_));
        }

    protected:
        ///
        /// \brief  Copying entities is restricted to internal library
        /// functions, as we don't want to encourage having
        ///         multiple instances of supposedly unique objects.
        ///
        /// \param e    Entity to be copied
        entity(const entity<entity_type_> &e)
        : identifier(e.identifier), children_(e.children_)
        {}

    public:
        virtual ~entity() = default;

        ///
        /// \param stream output-stream
        /// \param e entity
        /// \return reference to the now modified output-stream
        friend ostream &operator<<(ostream &stream,
                                   const entity<entity_type_> &e)
        {
            return stream << e.identifier;
        }

        ///
        /// \tparam archive_t
        /// \param archive
        /// \param version
        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &make_nvp("identifier",
                              const_cast<identity<entity_type_> &>(identifier));
            archive &BOOST_SERIALIZATION_NVP(children_);
        }
    };

}  // namespace esl

namespace std {
    ///
    /// Hash specification (standard library) for entity, using the entity's
    /// identifier \tparam entity_type_
    template<typename entity_type_>
    struct hash<esl::entity<entity_type_>>
    {
        size_t operator()(const esl::entity<entity_type_> &e) const
        {
            return std::hash<esl::identity<entity_type_>>()(e.identifier);
        }
    };
}  // namespace std


#ifdef WITH_MPI
#include <boost/mpi/datatype.hpp>
namespace boost::mpi {
    ///
    /// \brief Empty object, therefore trivially an mpi datatype
    ///
    template<>
    struct is_mpi_datatype<esl::entity<void>> : public mpl::true_
    {};

    ///
    /// \brief  Not an mpi datatype, as it has an identifier member variable
    /// (which is not an mpi dataype).
    /// \tparam entity_type_
    template<typename entity_type_>
    struct is_mpi_datatype<esl::entity<entity_type_>> : public mpl::false_
    {};

    ///
    /// \brief Empty object, therefore trivially an mpi dataype
    /// \tparam derived_type_
    template<typename derived_type_>
    struct is_mpi_datatype<esl::identifiable_as<derived_type_>>
    : public mpl::true_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_SIMULATION_ENTITY_HPP