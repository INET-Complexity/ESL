/// \file   demand_supply_function.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-04
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
#ifndef ESL_SUPPLY_DEMAND_FUNCTION_HPP
#define ESL_SUPPLY_DEMAND_FUNCTION_HPP

#include <array>
#include <vector>


#include <esl/economics/markets/quote.hpp>
// for fungible, agree on basis (fractions)
// for infungible, demand can logically be only 0 or 1...
// multiple dimensions, because we can have dependent demand (packages,
// constraints)


///
/// \brief                  excess demand (demand-supply) function dynamic in
/// the number of properties.
///
struct demand_supply_function
{
    virtual ~demand_supply_function() = default;

    ///
    /// \param quotes
    /// \param variables
    /// \return
    virtual std::vector<double>
    excess_demand(const std::vector<esl::economics::quote> &quotes,
                  const std::vector<double> &variables) const = 0;

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



///
/// \brief                  excess demand (demand-supply) function at the type
/// level.
///
/// \tparam property_ts_    the different properties that are covered by this
/// function (separate dimensions)
// template<typename ... property_ts_>
// struct demand_supply_function_t
//{
//    template<typename scalar_model_ = quantity>
//    array<scalar_model_, sizeof ... (property_ts_)> at(const array<quote,
//    sizeof ... (property_ts_)> &quotes) = 0;
//};


#ifdef WITH_MPI
namespace boost { namespace mpi {

    template<>
    struct is_mpi_datatype<demand_supply_function> : public mpl::true_
    {};

}}      // namespace boost::mpi
#endif  // WITH_MPI


///
/// \brief Concept definition of the demand function
///
/// \tparam T

// template<typename type_>
// concept demand_function = requires(type_ a)
//{
//    {  excess_demand<type_>{}(a) } -> quantity;
//};


#endif  // ESL_SUPPLY_DEMAND_FUNCTION_HPP
