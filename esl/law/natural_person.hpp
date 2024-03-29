/// \file   natural_person.hpp
///
/// \brief  Used to uniquely identify a natural person
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
#ifndef ESL_NATURAL_PERSON_HPP
#define ESL_NATURAL_PERSON_HPP

#include <iostream>

#include <esl/geography/iso_3166_1_alpha_2.hpp>

namespace esl::law {

    struct natural_person
    {
        ///
        /// \brief  primary nationality
        ///
        geography::iso_3166_1_alpha_2 nationality;

        natural_person(geography::iso_3166_1_alpha_2 nationality = geography::iso_3166_1_alpha_2())
        : nationality(nationality)
        {

        }

        virtual ~natural_person() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_NVP(nationality);
        }

        friend inline std::ostream &operator<<(std::ostream &o,
                                               const natural_person &np)
        {
            o << np.nationality;
            return o;
        }
    };
}  // namespace esl::law

#endif  // ESL_NATURAL_PERSON_HPP
