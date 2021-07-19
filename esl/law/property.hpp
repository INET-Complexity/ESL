/// \file   property.hpp
///
/// \brief  Things that can be owned
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-28
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
#ifndef ESL_LAW_PROPERTY_HPP
#define ESL_LAW_PROPERTY_HPP

#include <esl/simulation/entity.hpp>

#include <esl/computation/type_code.hpp>
#include <esl/computation/type_marker.hpp>


namespace esl::law {
    ///
    /// \brief  property is anything that can be owned
    ///
    struct property
    : public entity<property>
    , public type_code<0>
    , public type_marker<property>
    {
        explicit property(identity<property> i = identity<property>());

        virtual ~property() = default;

        ///
        /// \brief  Names the type of property
        /// // C++20 constexpr
        ///
        /// \return 
        [[nodiscard]] virtual std::string name() const;

        using esl::entity<property>::operator==;

        ///
        /// \tparam archive_t
        /// \param archive
        /// \param version
        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &boost::serialization::make_nvp(
                "entity⟨property⟩",
                boost::serialization::base_object<entity<property>>(*this));
        }
    };
}  // namespace esl::law

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::law::property>
    : mpl::true_
    {};
}  // namespace boost::mpi

#endif  // WITH_MPI

#endif  // ESL_PROPERTY_HPP
