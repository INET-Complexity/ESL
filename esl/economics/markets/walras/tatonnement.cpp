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

#include <esl/economics/markets/quote.hpp>
#include <esl/data/log.hpp>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_multimin.h>


#include <adept_source.h>

///
/// \brief  C compatible callback for the minimiser to find the function
///         value.
///
/// \param variables
/// \param params       Pointer to the tatonnement instance
///
extern "C" double
my_function_value(const gsl_vector *variables,
                  void *params)
{
    auto *model_ = static_cast<tatonnement::excess_demand_model *>(params);
    assert(model_ &&
               "parameter must be (tatonnement::excess_demand_model *)");
    return model_->calc_function_value(variables->data);
}

// Return gradient of function with respect to each state variable x
extern "C" void
my_function_gradient(const gsl_vector *x, void *params,
                     gsl_vector *gradJ)
{
    auto *model_ = static_cast<tatonnement::excess_demand_model *>(params);
    assert(model_ &&
               "parameter must be (tatonnement::excess_demand_model *)");
    model_->calc_function_value_and_gradient(x->data, gradJ->data);
}

///
/// \brief
///
extern "C" void my_function_value_and_gradient(
    const gsl_vector *x, void *params, double *J, gsl_vector *gradJ)
{
    auto *model_ = static_cast<tatonnement::excess_demand_model *>(params);
    assert(model_ &&
               "parameter must be (tatonnement::excess_demand_model *)");
    *J = model_->calc_function_value_and_gradient(x->data, gradJ->data);
}


namespace tatonnement {
    excess_demand_model::excess_demand_model(
        std::map<esl::identity<esl::law::property>, esl::economics::quote>
            initial_quotes)
    : quotes_(initial_quotes)
    , stack_()
    {

    }

    excess_demand_model::~excess_demand_model() = default;

    ///
    /// \brief the optimisation problem
    ///
    /// \param x
    /// \return
    adept::adouble
    excess_demand_model::calc_function_value(const adept::adouble *x)
    {
        std::map<esl::identity<esl::law::property>,
                 std::tuple<esl::economics::quote, adept::adouble>>
            quote_scalars_;

        size_t n = 0;
        for(auto [k, v]: quotes_) {
            quote_scalars_.insert({k, std::make_tuple(v, x[n])});
            ++n;
        }

        std::map<esl::identity<esl::law::property>, adept::adouble> terms_map;

        for(const auto &f : excess_demand_functions_) {
            auto demand_per_property_ = f->excess_demand_m(quote_scalars_);
            for(auto [k, v]: demand_per_property_) {
                if(terms_map.find(k) == terms_map.end()){
                    terms_map.emplace(k, v);
                }else{
                    (terms_map.find(k)->second) += v;
                }
            }
        }
        adept::adouble target_ = 0.0;
        for(auto [p, t] : terms_map) {
            (void)p;
            target_ += (pow(t, 2));
        }

        //LOG(notice) << " clearing error " << target_.value() << std::endl;
        return target_;
    }

    ///
    /// compute the function value without differentiation
    /// \param x
    /// \return
    double excess_demand_model::calc_function_value(const double *x)
    {
        stack_.pause_recording();
        for(unsigned int i = 0; i < active_x_.size(); ++i) {
            active_x_[i] = x[i];
        }
        double result = adept::value(calc_function_value(&active_x_[0]));
        stack_.continue_recording();
        return result;
    }

    double
    excess_demand_model::calc_function_value_and_gradient(const double *x,
                                                          double *dJ_dx)
    {
        for(unsigned int i = 0; i < active_x_.size(); ++i) {
            active_x_[i] = x[i];
        }

        stack_.new_recording();
        adept::adouble J = calc_function_value(&active_x_[0]);
        J.set_gradient(1.0);
        stack_.compute_adjoint();
        adept::get_gradients(&active_x_[0], active_x_.size(), dJ_dx);
        return adept::value(J);
    }

    std::optional<std::map<esl::identity<esl::law::property>, double>>
    excess_demand_model::do_compute()
    {
        const double initial_step_size       = 1.0e-4;
        const double line_search_tolerance   = 1.0e-4;
        const double converged_gradient_norm = 1.0e-3;
        const auto *minimizer_type =
            gsl_multimin_fdfminimizer_vector_bfgs2;  // l-bfgs

        active_x_.clear();
        std::vector<esl::identity<esl::law::property>> mapping_index_;

        mapping_index_.reserve(quotes_.size());
        {
            for(auto [k, v] : quotes_) {
                (void)v;
                mapping_index_.emplace_back(k);
                active_x_.push_back(1.0);
            }
        }

        gsl_multimin_function_fdf my_function;
        my_function.n      = active_x_.size();
        my_function.f      = my_function_value;
        my_function.df     = my_function_gradient;
        my_function.fdf    = my_function_value_and_gradient;
        my_function.params = static_cast<void *>(this);

        gsl_vector *x = gsl_vector_alloc(active_x_.size());
        for(size_t i = 0; i < active_x_.size(); ++i) {
            // initial solution is 1.0 * previous quote
            gsl_vector_set(x, i, 1.0);
        }

        auto *minimizer =
            gsl_multimin_fdfminimizer_alloc(minimizer_type, active_x_.size());
        gsl_multimin_fdfminimizer_set(minimizer, &my_function, x,
                                      initial_step_size, line_search_tolerance);

        size_t iter = 0;
        int status;
        do {
            ++iter;
            status = gsl_multimin_fdfminimizer_iterate(minimizer);

            std::vector<double> solution_;
            for(auto param = 0; param < active_x_.size(); ++param){
                solution_.push_back(gsl_vector_get (minimizer->x, param));
            }

            auto checkval = calc_function_value(&solution_[0]);

            //LOG(trace) << " CHECKVAL " << checkval << std::endl;
            if(checkval <= 0.01){
                //LOG(trace) << " good enough clearing error " << checkval << std::endl;
                status = GSL_SUCCESS;
                break;
            }

            if(status != GSL_SUCCESS) {
                break;
            }

            status = gsl_multimin_test_gradient(minimizer->gradient,
                                                converged_gradient_norm);
        } while(status == GSL_CONTINUE && iter < 1000);


        if(status == GSL_SUCCESS) {
            std::map<esl::identity<esl::law::property>, double> result_;
            for(size_t i = 0; i < active_x_.size(); ++i) {
                result_.insert({mapping_index_[i], gsl_vector_get (minimizer->x, i)});
            }


            gsl_multimin_fdfminimizer_free(minimizer);



            gsl_vector_free(x);

            return result_;
        }

        // no progress to a new solution, so use the old solution
        // n.b. this most frequently happens when we get the market clearing
        // prices right on the first try
        if(status == GSL_ENOPROG) {
            std::map<esl::identity<esl::law::property>, double> result_;
            for(size_t i = 0; i < active_x_.size(); ++i) {
                result_.insert({mapping_index_[i], gsl_vector_get (minimizer->x, i)});
            }

            gsl_multimin_fdfminimizer_free(minimizer);

            gsl_vector_free(x);
            return result_;
        }

        gsl_multimin_fdfminimizer_free(minimizer);

        gsl_vector_free(x);
        LOG(error)  << "minimizer failed after " << iter
                  << " iterations: " << gsl_strerror(status) << std::endl;
        return std::nullopt;
    }

}  // namespace tatonnement