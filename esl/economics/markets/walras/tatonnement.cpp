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

/*
#include <stan/callbacks/logger.hpp>
#include <stan/io/var_context.hpp>
#include <stan/services/optimize/lbfgs.hpp>
*/


#include <esl/economics/markets/quote.hpp>

// for L-BFGS with Adept

#include <gsl/gsl_vector.h>
#include <gsl/gsl_multimin.h>

#include <adept_source.h>

namespace tatonnement {

    extern "C"
    double excess_demand_model::my_function_value(const gsl_vector *variables, void *params)
    {
        auto *model_ = reinterpret_cast<tatonnement::excess_demand_model *>(params);

        return model_->calc_function_value(variables->data);
    }
// Return gradient of function with respect to each state variable x
    extern "C"
    void excess_demand_model::my_function_gradient(const gsl_vector *x, void *params, gsl_vector *gradJ)
    {
        auto *state = reinterpret_cast<tatonnement::excess_demand_model *>(params);

        state->calc_function_value_and_gradient(x->data, gradJ->data);
    }
// Return both function and its gradient
    extern "C"
    void excess_demand_model::my_function_value_and_gradient(const gsl_vector *x, void *params, double *J, gsl_vector *gradJ) {
        auto *state = reinterpret_cast<tatonnement::excess_demand_model *>(params);

        *J = state->calc_function_value_and_gradient(x->data, gradJ->data);
    }

}//namespace tatonnement









namespace tatonnement {
    excess_demand_model::excess_demand_model( std::map<esl::identity<esl::law::property>
                                            , esl::economics::quote> initial_quotes)
    : quotes_(initial_quotes)
    {

    }

    excess_demand_model::~excess_demand_model() = default;

    ///
    /// \brief the optimisation problem
    ///
    /// \param x
    /// \return
    adept::adouble excess_demand_model::calc_function_value(const adept::adouble *x)
    {

        std::map< esl::identity<esl::law::property>
                , std::tuple<esl::economics::quote, adept::adouble>
        > quote_scalars_;

        std::vector<adept::adouble> scalars_;
        scalars_.reserve(quotes_.size());
        size_t n = 0;
        for(auto [k,v]: quotes_){
            quote_scalars_.insert({k, std::make_tuple(v, x[n])});
            scalars_.push_back(x[n]);
            ++n;
        }

        std::map< esl::identity<esl::law::property>, adept::adouble> terms_map;
        for(auto [k,v]: quotes_) {
            terms_map.insert({k, adept::adouble(0)});
        }

        for(const auto &f: excess_demand_functions_) {
            auto demand_per_property_ = f->excess_demand_m(quote_scalars_);
            for(auto [k,v]: demand_per_property_) {
                terms_map[k] += v;
            }
        }

        adept::adouble target_ = 0.0;
        for(auto [p,t]: terms_map) {
            (void) p;
            target_ += (pow(t, 2));
        }

        return target_;
    }

    ///
    /// compute the function value without differentiation
    /// \param x
    /// \return
    double excess_demand_model::calc_function_value(const double* x)
    {
        stack_.pause_recording();
        for (unsigned int i = 0; i < active_x_.size(); ++i) {
            active_x_[i] = x[i];
        }
        double result = value(calc_function_value(&active_x_[0]));
        stack_.continue_recording();
        return result;
    }

    double excess_demand_model::calc_function_value_and_gradient(const double* x, double* dJ_dx) {
        for (unsigned int i = 0; i < active_x_.size(); ++i){
            active_x_[i] = x[i];
        }
        stack_.new_recording();
        adept::adouble J = calc_function_value(&active_x_[0]);
        J.set_gradient(1.0);
        stack_.compute_adjoint();
        adept::get_gradients(&active_x_[0], active_x_.size(), dJ_dx);
        return adept::value(J);
    }

    std::optional<std::map<esl::identity<esl::law::property>, double>> excess_demand_model::do_compute()
    {
        const double initial_step_size = 0.01;
        const double line_search_tolerance = 1.0e-4;
        const double converged_gradient_norm = 1.0e-3;
        const auto *minimizer_type = gsl_multimin_fdfminimizer_vector_bfgs2;// l-bfgs

        active_x_.clear();
        std::vector<esl::identity<esl::law::property>> mapping_index_;
        mapping_index_.reserve(quotes_.size());
        {
            for (auto [k, v]: quotes_) {
                (void)v;
                mapping_index_.emplace_back(k);
                active_x_.emplace_back(1.0);
            }
        }

        gsl_multimin_function_fdf my_function;
        my_function.n = active_x_.size();
        my_function.f = my_function_value;
        my_function.df = my_function_gradient;
        my_function.fdf = my_function_value_and_gradient;
        my_function.params = reinterpret_cast<void *>(this);

        auto *x = gsl_vector_alloc(active_x_.size());
        for (size_t i = 0; i < active_x_.size(); ++i){
            gsl_vector_set(x, i, 1.0);  // initial solution is 1.0 * previous quote
        }

        auto *minimizer = gsl_multimin_fdfminimizer_alloc(minimizer_type, active_x_.size());
        gsl_multimin_fdfminimizer_set(minimizer, &my_function, x, initial_step_size, line_search_tolerance);

        size_t iter = 0;
        int status;
        do {
            ++iter;
            status = gsl_multimin_fdfminimizer_iterate(minimizer);
            if (status != GSL_SUCCESS) break;
            status = gsl_multimin_test_gradient(minimizer->gradient, converged_gradient_norm);
        }
        while (status == GSL_CONTINUE && iter < 1000);

        gsl_multimin_fdfminimizer_free(minimizer);
        gsl_vector_free(x);

        if (status == GSL_SUCCESS) {
            std::map<esl::identity<esl::law::property>, double> result_;
            for(size_t i = 0; i < active_x_.size(); ++i){
                result_.insert({mapping_index_[i], active_x_[i].value()});
            }
            return result_;
        }

        std::cout << "Minimizer failed after " << iter << " iterations: " << gsl_strerror(status) << "\n";
        return std::nullopt;
    }

}  // namespace tatonnement