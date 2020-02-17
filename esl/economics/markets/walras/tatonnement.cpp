/// \file   tatonnement.hpp
///
/// \brief  Implements the tâtonnement process (hill climbing), implemented as a
///         numerical optimisation (L-BFGS) with
///         automatic differentiation using the Stan-math library.
///
/// \remark This code uses the spelling `tatonnement`, as the accent on `â` can
///         not be rendered in the default
///         std::string.
///
/// \authors    Maarten P. Scholl
/// \date       2018-02-02
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

#include <esl/economics/markets/walras/tatonnement.hpp>

#include <stan/callbacks/logger.hpp>
#include <stan/io/var_context.hpp>
#include <stan/services/optimize/lbfgs.hpp>

#include <esl/economics/markets/quote.hpp>

namespace tatonnement {
    excess_demand_model::excess_demand_model(
        std::map<esl::identity<esl::law::property>, esl::economics::quote>
            initial_quotes)
    : prob_grad(initial_quotes.size()), quotes_(std::move(initial_quotes))
    {
        param_ranges_i__.clear();  /// no restricted integer ranges
    }

    excess_demand_model::~excess_demand_model() = default;

    void excess_demand_model::transform_inits(
        const stan::io::var_context &context, std::vector<int> &integers,
        std::vector<double> &reals, std::ostream *stream) const
    {
        (void)stream;
        stan::io::writer<double> writer_(reals, integers);

        std::vector<double> prices_ = context.vals_r("prices");
        for(auto &p : prices_) {
            writer_.scalar_unconstrain(p);
        }

        reals    = writer_.data_r();
        integers = writer_.data_i();
    }

    void excess_demand_model::transform_inits(
        const stan::io::var_context &context,
        Eigen::Matrix<double, Eigen::Dynamic, 1> &reals,
        std::ostream *stream) const
    {
        std::vector<int> params_i_vec;
        std::vector<double> params_r_vec;
        transform_inits(context, params_i_vec, params_r_vec, stream);

        reals = Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, 1>>(
            params_r_vec.data(), params_r_vec.size(), 1);
    }

    /// \param names[out]   Is set to the names of the parameters to the model
    void
    excess_demand_model::get_param_names(std::vector<std::string> &names) const
    {
        names = {"prices"};
    }

    /// \param dimensions[out]  The dimensions of each parameter.
    void excess_demand_model::get_dims(
        std::vector<std::vector<size_t>> &dimensions) const
    {
        dimensions = {{quotes_.size()}};
    }


    void excess_demand_model::constrained_param_names(
        std::vector<std::string> &names, bool transformed_parameters,
        bool generated_quantities) const
    {
        (void)transformed_parameters;
        (void)generated_quantities;

        names.clear();
        names.reserve(quotes_.size());
        for(size_t i = 0; i < quotes_.size(); ++i) {
            names.emplace_back("prices." + std::to_string(1 + i));
        }
    }

    void excess_demand_model::unconstrained_param_names(
        std::vector<std::string> &names, bool transformed_parameters,
        bool generated_quantities) const
    {
        (void)transformed_parameters;
        (void)generated_quantities;

        names.clear();
        names.reserve(quotes_.size());
        for(size_t i = 0; i < quotes_.size(); ++i) {
            names.emplace_back("prices." + std::to_string(1 + i));
        }
    }
}  // namespace tatonnement