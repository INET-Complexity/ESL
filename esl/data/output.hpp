/// \file   output.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-03-14
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
#ifndef ESL_DATA_OUTPUT_HPP
#define ESL_DATA_OUTPUT_HPP

#include <string>
#include <iostream>
#include <vector>

#include <boost/serialization/vector.hpp>

#include <esl/data/output_base.hpp>
#include <esl/simulation/time.hpp>


namespace esl::data {
    ///
    /// \addtogroup simulation
    /// \brief  An "output" is some variable that is strictly written to from
    ///         inside the simulation.
    ///
    /// \details
    ///
    ///
    template<typename... variable_types_>
    struct output
    : public output_base
    {
        friend class boost::serialization::access;

    protected:
        ///
        /// \brief The (process-local) observed history of values.
        ///
        std::vector<std::tuple<simulation::time_point, variable_types_...>> values;

    public:
        ///
        /// \brief  Whether the output
        ///
        bool eager;

        ///
        ///
        ///
        output(const std::string &name =
                   ("observable_" + std::to_string(sizeof...(variable_types_))), bool eager = true)
        : output_base(name)
        , values()
        , eager(eager)
        {

        }

        ///
        /// \param t
        /// \param v
        void put(simulation::time_point t, variable_types_... v)
        {
            values.emplace_back(
                std::tuple<simulation::time_point, variable_types_...>(t, v...));
            if(eager){
                (write(v), ...);
            }
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            //archive.template register_type<simulation::time_point>();
            archive.template register_type<variable_types_...>();
            archive.template register_type<output<variable_types_...>>();
            archive.template register_type<std::tuple<simulation::time_point, variable_types_...>>();

            archive &boost::serialization::make_nvp(
                "output_base",
                boost::serialization::base_object<output_base>(*this));

            archive &BOOST_SERIALIZATION_NVP(values);
        }
    };
}  // namespace esl::data



#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<typename... variable_types_>
    struct is_mpi_datatype<esl::data::output<variable_types_...>>
        : public mpl::false_
    {

    };
}  // namespace boost::mpi
#endif  // WITH_MPI



#endif  // ESL_DATA_OUTPUT_HPP
