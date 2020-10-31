/// \file   timing.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-11-23
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
#ifndef ESL_COMPUTATION_TIMING_HPP
#define ESL_COMPUTATION_TIMING_HPP

#include <chrono>

#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/nvp.hpp>


namespace esl::computation {

    ///
    /// \brief  Tests if the provided clock has at least millisecond precision
    ///
    /// \details    This integer fraction tells us the precision in seconds,
    ///             therefore we test the reciprocal.
    ///
    static_assert(std::chrono::high_resolution_clock::period::den
                      / std::chrono::high_resolution_clock::period::num
                  >= 1000);

    ///
    ///
    ///
    struct agent_timing
    {
        std::chrono::milliseconds messaging;
        std::chrono::milliseconds acting;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;

            boost::serialization::make_nvp(
                "messaging",
                boost::serialization::make_binary_object(&messaging,
                                                         sizeof(messaging)));

            boost::serialization::make_nvp(
                "acting",
                boost::serialization::make_binary_object(&acting,
                                                         sizeof(acting)));
        }
    };
}  // namespace esl::computation


#ifdef WITH_MPI
#include <boost/mpi.hpp>

namespace boost::mpi {
    ///
    /// \details    POD structures can be mapped to MPI datatypes therefore
    ///             this derives true_
    template<>
    struct is_mpi_datatype<esl::computation::agent_timing>
        : public mpl::true_
    {};
}  // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_COMPUTATION_TIMING_HPP
