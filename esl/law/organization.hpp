/// \file   organisation.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-03-23
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
#ifndef ESL_ORGANIZATION_HPP
#define ESL_ORGANIZATION_HPP

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>

#include <esl/law/legal_person.hpp>


namespace esl::law {

    struct jurisdiction;

    struct organization
    : public legal_person
    {
    private:
        friend class boost::serialization::access;

    public:
        organization(const identity<agent> &i,
                     const jurisdiction &primary_jurisdiction)
        : agent(i), legal_person(i, primary_jurisdiction)
        {

        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(legal_person);
        }



    };
}

#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::law::organization>
    : is_mpi_datatype<esl::law::legal_person>::type
    {

    };
}
#endif  // WITH_MPI

#endif  // ESL_ORGANIZATION_HPP
