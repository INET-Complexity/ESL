/// \file   jurisdiction.hpp
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
#ifndef ESL_JURISDICTION_HPP
#define ESL_JURISDICTION_HPP

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>

#include <esl/geography/iso_3166_1_alpha_2.hpp>
#include <esl/economics/iso_4217.hpp>

namespace esl::law {

    ///
    /// \brief  A jurisdiction describes an authority in a specific field.
    ///
    /// \details    At the highest level, we model authorities to administer
    ///             countries, and we can derive from this class to model
    ///             more specific regions such as federal states, provinces and
    ///             autonomous economic regions.
    ///
    struct jurisdiction
    {
        const geography::iso_3166_1_alpha_2 sovereign;

        const economics::iso_4217 tender;

        constexpr jurisdiction(geography::iso_3166_1_alpha_2 sovereign,
                               economics::iso_4217 tender)
        : sovereign(sovereign)
        , tender(tender)
        {

        }

        ~jurisdiction() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &boost::serialization::make_nvp(
                "sovereign", const_cast<geography::iso_3166_1_alpha_2 &>(sovereign));
            archive &boost::serialization::make_nvp(
                "tender", const_cast<economics::iso_4217 &>(tender));
        }
    };

}  // namespace esl::law

namespace std {
    template<>
    struct hash<esl::law::jurisdiction>
    {
        constexpr size_t operator()(const esl::law::jurisdiction &j) const
        {
            return hash<esl::geography::iso_3166_1_alpha_2>()(j.sovereign);
        }
    };
}  // namespace std


#ifdef WITH_MPI
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::law::jurisdiction>
    : public mpl::true_
    {

    };
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_JURISDICTION_HPP
