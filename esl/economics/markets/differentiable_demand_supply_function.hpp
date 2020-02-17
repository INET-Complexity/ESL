/// \file   differentiable_demand_supply_function.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-21
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
#ifndef ESL_DIFFERENTIABLE_DEMAND_SUPPLY_FUNCTION_HPP
#define ESL_DIFFERENTIABLE_DEMAND_SUPPLY_FUNCTION_HPP

#include <esl/economics/markets/demand_supply_function.hpp>
#include <esl/mathematics/variable.hpp>



struct differentiable_demand_supply_function
    : public demand_supply_function
{
    virtual ~differentiable_demand_supply_function() = default;


    
    virtual std::map<esl::identity<esl::law::property>, esl::variable>
    excess_demand_m(const std::map< esl::identity<esl::law::property>
                                  , std::tuple<esl::economics::quote, esl::variable>
                                  > &quotes) const = 0;

    ///
    /// \brief  Shim to convert from auto-differentiated variables to double
    ///
    virtual std::map<esl::identity<esl::law::property>, double> excess_demand_m(
        const std::map<esl::identity<esl::law::property>,
                       std::tuple<esl::economics::quote, double>> &quotes)
        const override
    {

        std::map<esl::identity<esl::law::property>,
                 std::tuple<esl::economics::quote, esl::variable>> quotes_;
        for(auto [k, v]: quotes) {
            quotes_.insert({ k
                           , std::make_tuple( std::get<0>(v)
                                            , esl::variable(std::get<1>(v)))
                           });
        }

        std::map<esl::identity<esl::law::property>, esl::variable> intermediate_ = excess_demand_m(quotes_);

        std::map<esl::identity<esl::law::property>, double> result_;
        for(auto [k,v]: intermediate_) {
            result_.insert({k, v.val()});
        }
        return result_;
    }





    virtual std::vector<esl::variable>
    excess_demand(const std::vector<esl::economics::quote> &quotes,
                  std::vector<esl::variable> &multipliers) const = 0;

    ///
    /// \brief  Shim to convert from auto-differentiated variables to double
    ///
    virtual std::vector<double>
    excess_demand(const std::vector<esl::economics::quote> &quotes,
                  const std::vector<double> &variables) const override
    {
        std::vector<esl::variable> variables_;
        variables_.reserve(variables.size());
        for(auto v : variables) {
            variables_.emplace_back(esl::variable(v));
        }

        std::vector<esl::variable> intermediate_ =
            excess_demand(quotes, variables_);

        std::vector<double> result_;
        result_.reserve(intermediate_.size());
        for(auto d : intermediate_) {
            result_.emplace_back(d.val());
        }
        return result_;
    }


    template<class archive_t>
    void serialize(archive_t &archive, const unsigned int version)
    {
        (void)archive;
        (void)version;
    }
};




#ifdef WITH_MPI
namespace boost { namespace mpi {

        template<>
        struct is_mpi_datatype<differentiable_demand_supply_function>
            : public mpl::true_
        {};

    }}      // namespace boost::mpi
#endif  // WITH_MPI

#endif  // ESL_DIFFERENTIABLE_DEMAND_SUPPLY_FUNCTION_HPP
