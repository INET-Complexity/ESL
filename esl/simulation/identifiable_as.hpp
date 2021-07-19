/// \file   identifiable_as.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-25
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
#ifndef ESL_SIMULATION_IDENTIFIABLE_AS_HPP
#define ESL_SIMULATION_IDENTIFIABLE_AS_HPP

#include <type_traits>

#include <esl/simulation/identity.hpp>


namespace esl {
    // forward declaration, see simulation/entity.hpp
    template<typename entity_type_>
    struct entity;

    ///
    /// \brief  Given a type derived_type_, that is required to also inherit
    ///         directly or indirectly entity<>, uniquely
    ///         identifiable by a *type safe* identifier_t<derived_type_>. This
    ///         is useful for when we want to restrict types to identifiers of a
    ///         specific derived type.
    ///
    /// \example    Type D inherits base B. B is an entity<B>, and hence has an
    ///             identifier_t<B>. We now want a function 'f'
    ///             that is restricted to identifiers of type identifier_t<D>.
    ///             This requires dynamic type information, for example when
    ///             dealing with pointers to type entity<B>, as not necessarily
    ///             all are convertible to pointers of type entity<D>.
    ///
    /// \tparam derived_type_
    ///
    template<typename derived_type_>
    struct identifiable_as
    {
        identifiable_as() = default;

        virtual ~identifiable_as() = default;

        identifiable_as<derived_type_> &operator = (const identifiable_as<derived_type_> &) = default;

        ///
        /// \brief  Uses dynamic type information to cast to identifiers of a
        /// derived type. The derived type must directly
        ///         or indirectly inherit detail::elementary_entity
        /// \return The identifier of derived type, or throws a std::bad_cast
        /// exception due to the use of dynamic_cast.
        constexpr operator identity<derived_type_>() const
        {
            //static_assert(std::is_base_of<entity<void>, derived_type_>::value,
            //              "derived_type_ must inherit entity for this cast");
            return identity<derived_type_>(
                    //(dynamic_cast<const derived_type_ *>(this))->identifier.digits
                (dynamic_cast<const derived_type_ *>(this))->identifier.digits
                );
        }

        ///
        /// \tparam archive_t
        /// \param archive
        /// \param version
        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)archive;
            (void)version;
        }
    };
}  // namespace esl




#ifdef WITH_MPI

#include <boost/mpi.hpp>
namespace boost ::mpi {
    ///
    /// \brief Empty object, therefore trivially an mpi datatype
    /// \tparam derived_type_
    template<typename derived_type_>
    struct is_mpi_datatype<esl::identifiable_as<derived_type_>>
        : public mpl::true_
    {};
}
#endif//WITH_MPI

#endif  // ESL_SIMULATION_IDENTIFIABLE_AS_HPP
