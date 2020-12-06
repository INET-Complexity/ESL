/// \file   tatonnement.hpp
///
/// \brief  Implements the tâtonnement process (hill climbing), implemented as a
///         numerical optimisation.
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

#include <gsl/gsl_vector.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_multiroots.h>

#pragma warning(push, 0)  // supress warnings in MSVC from external code
#include <adept_source.h>
#pragma warning(pop)

#include <esl/economics/markets/quote.hpp>
#include <esl/data/log.hpp>
#include <esl/mathematics/variable.hpp>
#include <esl/invalid_parameters.hpp>
using esl::economics::markets::tatonnement::excess_demand_model;
using esl::/*mathematics::*/variable;

///
/// \brief  C compatible callback for the minimizer to find the function value.
///
/// \param variables
/// \param params       Pointer to the excess_demand_model instance
///
extern "C" double c_minimizer_function_value
    ( const gsl_vector *variables
        , void *model)
{
    auto *model_ = static_cast<excess_demand_model *>(model);
    assert(model_ && "parameter must be (excess_demand_model *)");
    return model_->excess_demand_function_value(variables->data);
}

///
/// \req    this function should store the vector result f(x,params)
///         in f for argument x and parameters params, returning an appropriate
///         error code if the function cannot be computed.
///
///
extern "C" int
multiroot_function_value_cb(const gsl_vector *x, void *params, gsl_vector *f)
{
    auto *model_ = static_cast<excess_demand_model *>(params);
    assert(model_ && "parameter must be (excess_demand_model *)");
    auto cb_ = model_->multiroot_function_value(x->data);

    for(size_t i = 0; i < cb_.size(); ++i){
        gsl_vector_set(f, i, cb_[i]);
    }

    return GSL_SUCCESS;
}

#if !defined(ADEPT_VERSION) || !defined(ADEPT_NO_AUTOMATIC_DIFFERENTIATION)

///
/// \brief  C wrapper for minimization problem, gsl callback
///         Return gradient of function with respect to each state variable x
///
extern "C" void c_minimizer_function_gradient( const gsl_vector *x
    , void *params
    , gsl_vector *gradient)
{
    auto *model_ = static_cast<excess_demand_model *>(params);
    assert(model_ && "parameter must be (excess_demand_model *)");
    model_->minimizer_function_value_and_gradient(x->data, gradient->data);
}

///
/// \req    this function should store the n-by-n matrix result
///         J_{ij} = \partial f_i(x,\hbox{\it params}) / \partial x_j
///         in J for argument x and parameters params, returning an appropriate
///         error code if the function cannot be computed.
///
///
extern "C" int multiroot_function_jacobian_cb(const gsl_vector * x, void * params, gsl_matrix * df)
{
    auto *model_ = static_cast<excess_demand_model *>(params);
    assert(model_ && "parameter must be (excess_demand_model *)");
    auto cb_ = model_->multiroot_function_value_and_gradient(x->data, df->data);
    return GSL_SUCCESS;
}

///
/// \brief C wrapper for minimization problem
///
extern "C" void c_minimizer_function_value_and_gradient
    ( const gsl_vector *x
        , void *params
        , double *jacobian
        , gsl_vector *gradient
    )
{
    auto *model_ = static_cast<excess_demand_model *>(params);
    assert(model_ && "parameter must be (excess_demand_model *)");
    *jacobian = model_->minimizer_function_value_and_gradient(x->data, gradient->data);
}

///
/// \brief
///
extern "C" int multiroot_function_value_and_gradient_cb( const gsl_vector *x
    , void *params
    , gsl_vector *f
    , gsl_matrix *df
)
{
    auto *model_ = static_cast<excess_demand_model *>(params);
    assert(model_ && "parameter must be (excess_demand_model *)");

    auto cb_ = model_->multiroot_function_value_and_gradient(x->data, df->data);

    for(size_t i = 0; i < cb_.size(); ++i){
        gsl_vector_set(f, i, cb_[i]);
    }
    return GSL_SUCCESS;
}

extern "C" double uniroot_function_value (double x, void *params)
{
    auto *model_ = static_cast<excess_demand_model *>(params);
    assert(model_ && "parameter must be (excess_demand_model *)");

    auto cb_ = model_->multiroot_function_value(&x);

    return cb_[0];
}

extern "C" double uniroot_function_value_and_gradient (double x, void *params)
{
    auto *model_ = static_cast<excess_demand_model *>(params);
    assert(model_ && "parameter must be (excess_demand_model *)");

    double df = 0.;
    auto cb_ = model_->multiroot_function_value_and_gradient(&x, &df);

    return df;
}

extern "C" void uniroot_function_jacobian_cb ( double x
    , void *params
    , double *f
    , double *df)
{
    auto *model_ = static_cast<excess_demand_model *>(params);
    assert(model_ && "parameter must be (excess_demand_model *)");

    double jacobian_ = 0.;
    auto cb_ = model_->multiroot_function_value_and_gradient(&x, &jacobian_);

    *f = cb_[0];
    if(!std::isfinite(jacobian_)){
        jacobian_ = (x-1);
    }
    *df = jacobian_;
}
#endif

///
/// \brief  Callback for errors from the optimizer.
/// \details    We silence errors
///
/// \param reason
/// \param file
/// \param line
/// \param gsl_errno
void handler ([[maybe_unused]] const char *reason,
              [[maybe_unused]] const char *file,
              [[maybe_unused]] int line,
              [[maybe_unused]] int gsl_errno)
{

}

namespace esl::economics::markets::tatonnement {
    ///
    /// \brief  Initializes a stack for automatic differentiation
    ///
    /// \param initial_quotes
    excess_demand_model::excess_demand_model(
        law::property_map<quote> initial_quotes)
        : quotes(initial_quotes)
#if defined(ADEPT_VERSION)
        , stack_()
#endif
    {

    }

    excess_demand_model::~excess_demand_model() = default;

    ///
    /// \brief The optimisation version of the market clearing problem,
    ///         with automatic differentiation
    ///
    /// \param x
    /// \return
    adept::adouble excess_demand_model::demand_supply_mismatch(const adept::adouble *x)
    {
        std::map<identity<law::property>,
            std::tuple<quote, adept::adouble>>
            quote_scalars_;

        size_t n = 0;
        for(auto [k, v]: quotes) {
            quote_scalars_.emplace(*k, std::make_tuple(v, x[n]));
            ++n;
        }

        std::map<identity<law::property>, adept::adouble> terms_map;

        for(const auto &f : excess_demand_functions_) {
            auto demand_per_property_ = f->excess_demand(quote_scalars_);

            for(auto [k, v]: demand_per_property_) {
                if(terms_map.find(k) == terms_map.end()){
                    terms_map.emplace(k,  v);
                }else{
                    (terms_map.find(k)->second) += v;
                }
            }
        }
        variable target_ = 0.0;
        for(auto [p, t] : terms_map) {
            (void)p;
            target_ += (pow(t, 2));
        }

        //LOG(notice) << " clearing error " << target_.value() << std::endl;
        return target_;
    }

    ///
    /// \brief  Root-finding version of the market clearing problem.
    ///
    /// \details    Tries to set excess demand for all goods to zero, for all
    ///             goods individually.
    /// \param x
    /// \return
    std::vector<variable>
    excess_demand_model::excess_demand(const variable *x)
    {
        std::map<identity<law::property>, std::tuple<quote, variable>> quote_scalars_;
        size_t n = 0;
        for(auto [k, v]: quotes) {
            quote_scalars_.emplace(*k, std::make_tuple(v, x[n]));
            ++n;
        }

        std::map<identity<law::property>, variable> terms_map;
        for(const auto &f : excess_demand_functions_) {
            auto demand_per_property_ = f->excess_demand(quote_scalars_);
            for(auto [k, ed]: demand_per_property_) {
                auto i = terms_map.emplace(k, variable(0.));
                auto long_ = double(std::get<0>(f->supply[k]));
                auto short_ = double(std::get<1>(f->supply[k]));
                i.first->second += ed;//long_ + ed - short_;
            }
        }

        std::vector<variable> result_;
        for(auto [k, v]: quotes) {
            assert(terms_map.end() != terms_map.find(*k));
            result_.push_back(terms_map.find(*k)->second);
        }
        return result_;
    }

    ///
    /// \brief  Used to convert the optimization version of the problem back to
    ///         machine double precision floats.
    ///         Also uused when not using automatic differentiation.
    ///
    /// \param multipliers  Price multipliers
    /// \return
    double excess_demand_model::excess_demand_function_value(const double *multipliers)
    {
        stack_.pause_recording();
        for(unsigned int i = 0; i < active_.size(); ++i) {
            active_[i] = multipliers[i];
        }
        double result = adept::value(demand_supply_mismatch(&active_[0]));
        stack_.continue_recording();
        return result;
    }

    ///
    /// \brief  Wraps the root finding problem (value only)
    ///
    /// \param x
    /// \return
    std::vector<double> excess_demand_model::multiroot_function_value(const double *multipliers)
    {
        stack_.pause_recording();
        for(unsigned int i = 0; i < active_.size(); ++i) {
            active_[i] = multipliers[i];
        }
        auto intermediate_ = excess_demand(&active_[0]);
        std::vector<double> result;
        for(const auto &v: intermediate_){
            result.push_back(adept::value(v));
        }
        stack_.continue_recording();
        return result;
    }

    ///
    /// \brief wrapper for minimization problem, value and gradient
    ///
    /// \param x
    /// \param dJ_dx
    /// \return
    double
    excess_demand_model::minimizer_function_value_and_gradient(const double *multipliers, double *derivatives)
    {
        for(unsigned int i = 0; i < active_.size(); ++i) {
            active_[i] = multipliers[i];
        }

        stack_.new_recording();
        adept::adouble derivative_ = demand_supply_mismatch(&active_[0]);
        // in the minimization problem, the output is a single scalar
        derivative_.set_gradient(1.0);

        stack_.compute_adjoint();
        adept::get_gradients(&active_[0], active_.size(), derivatives);
        return adept::value(derivative_);
    }

    ///
    /// \brief   Root-finding version, value and gradient wrapper.
    ///
    /// \param multipliers  Price multipliers
    /// \param jacobian output for jacobian matrix
    /// \return
    std::vector<double>
    excess_demand_model::multiroot_function_value_and_gradient
        ( const double *multipliers
            , double *jacobian
        )
    {
        for(unsigned int i = 0; i < active_.size(); ++i) {
            active_[i] = multipliers[i];
        }

        stack_.new_recording();
        auto values_ = excess_demand(&active_[0]);

        stack_.independent(&active_[0], active_.size());
        stack_.dependent(&values_[0], values_.size());
        stack_.jacobian(jacobian);

        std::vector<double> result_;
        for(auto &v : values_) {
            result_.emplace_back(adept::value(v));
        }
        return result_;
    }

    ///
    /// \brief  Goes through the selected solution methods and applies them.
    ///
    /// \return numerical approximate multipliers for the quotes
    std::optional<std::map<identity<law::property>, double>>
    excess_demand_model::compute_clearing_quotes(size_t max_iterations)
    {
        if(methods.empty()){
            const auto error_no_solvers_ = "no solution method specified";
            LOG(errorlog) << error_no_solvers_ << std::endl;
            throw esl::invalid_parameters(error_no_solvers_);
        }

        for(auto method_: methods){
            // for every method we try, we need to reset our variables
            active_.clear();
            std::vector<identity<law::property>> mapping_index_;
            mapping_index_.reserve(quotes.size());
            // initial values are the solutions at the previous time step, or
            // the initial values provided to the model if this is the first
            for(auto [k, v] : quotes) {
                (void) v;
                mapping_index_.emplace_back(*k);
                active_.emplace_back(1.0 );
            }

            //  root finding methods try to set excess demand to zero for
            //  all properties traded in the market
            if (method_ == root){
#if !defined(ADEPT_VERSION) || !defined(ADEPT_NO_AUTOMATIC_DIFFERENTIATION)
                // if there is only one property traded, we specialize with
                // algorithms that do well on univariate root finding
                if(1 == quotes.size()){
                    constexpr double absolute_tolerance = 1e-6;
                    constexpr double delta_absolute_tolerance = 1e-6;
                    constexpr double delta_relative_tolerance = 1e-8;

                    // capture previous error handler to restore later
                    auto old_handler_ = gsl_set_error_handler (&handler);

                    // solver and solver type
                    const gsl_root_fdfsolver_type *solver_type_;
                    gsl_root_fdfsolver *s;
                    // the initial guess is 1 times the previous quote
                    double xt = 1.;
                    // the function structure with gradient and jacobian
                    gsl_function_fdf target_;
                    target_.f = &uniroot_function_value;
                    target_.df = &uniroot_function_value_and_gradient;
                    target_.fdf = &uniroot_function_jacobian_cb;
                    target_.params = static_cast<void *>(this);

                    // use steffenson's method
                    // Newton algorithm with an Aitken "delta-squared" acceleration
                    solver_type_ = gsl_root_fdfsolver_steffenson;
                    s = gsl_root_fdfsolver_alloc (solver_type_);
                    gsl_root_fdfsolver_set (s, &target_, xt);
                    size_t iteration_ = 0;

                    int status_;
                    double best_quote_ = 1.0;
                    double best_error_ = uniroot_function_value(best_quote_, this);//std::numeric_limits<double>::max();
                    do  {
                        ++iteration_;
                        // perform one solver step
                        status_ = gsl_root_fdfsolver_iterate(s);
                        // store initial position to compute delta
                        xt = gsl_root_fdfsolver_root(s);
                        auto errors_ = uniroot_function_value(xt, this);
                        if(abs(errors_) < abs(best_error_)){
                            best_error_ = errors_;
                            best_quote_ = xt;
                        }
                        if(abs(errors_) < absolute_tolerance){
                            status_ = GSL_SUCCESS;
                            break;
                        }
                        //status_ = gsl_root_test_delta (xt, x0, delta_absolute_tolerance, delta_relative_tolerance);
                    } while (GSL_CONTINUE == status_ && iteration_ < max_iterations);

                    if (GSL_SUCCESS == status_) {
                        std::map<esl::identity<esl::law::property>, double> result_;
                        // apply circuit breaker retro-actively
                        best_quote_ = std::max(best_quote_, circuit_breaker.first);
                        best_quote_ = std::min(best_quote_, circuit_breaker.second);

                        result_.emplace(mapping_index_[0], best_quote_);
                        gsl_root_fdfsolver_free (s);
                        return result_;
                    }
                    gsl_root_fdfsolver_free (s);
                    // reset the old error handler.
                    gsl_set_error_handler(old_handler_);
                } else {
                    // else, we solve for multiple roots at once
                    // TODO: set adaptively
                    constexpr double residual_tolerance = 1e-4;

                    gsl_multiroot_function_fdf target_;
                    target_.n = active_.size();
                    target_.f = &multiroot_function_value_cb;
                    target_.df = &multiroot_function_jacobian_cb;
                    target_.fdf = &multiroot_function_value_and_gradient_cb;
                    target_.params = static_cast<void *>(this);

                    gsl_vector *variables_ = gsl_vector_alloc(active_.size());
                    for (size_t i = 0; i < active_.size(); ++i) {
                        // initial solution is 1.0 times previous prices
                        gsl_vector_set(variables_, i, 1.);
                    }

                    // modified version of Powell’s Hybrid method
                    const gsl_multiroot_fdfsolver_type *solver_t_ = gsl_multiroot_fdfsolver_hybridsj;
                    gsl_multiroot_fdfsolver *solver_ = gsl_multiroot_fdfsolver_alloc(solver_t_, active_.size());
                    gsl_multiroot_fdfsolver_set(solver_, &target_, variables_);

                    int status = GSL_CONTINUE;
                    for (size_t i = 0; i < max_iterations && GSL_CONTINUE == status; ++i) {
                        status = gsl_multiroot_fdfsolver_iterate(solver_);
                        if (GSL_SUCCESS != status) {
                            break;
                        }
                        status = gsl_multiroot_test_residual(solver_->f, residual_tolerance);
                    }

                    if (GSL_SUCCESS == status) {
                        std::map<esl::identity<law::property>, double> result_;
                        auto solver_best_ = gsl_multiroot_fdfsolver_root(solver_);
                        for (size_t i = 0; i < active_.size(); ++i) {
                            auto scalar_ = gsl_vector_get(solver_best_, i);
                            // apply circuit breaker
                            scalar_ = std::min(scalar_, circuit_breaker.second);
                            scalar_ = std::max(scalar_, circuit_breaker.first);
                            result_.emplace(mapping_index_[i], scalar_);
                        }
                        gsl_multiroot_fdfsolver_free(solver_);
                        gsl_vector_free(variables_);
                        return result_;
                    }
                    gsl_multiroot_fdfsolver_free(solver_);
                    gsl_vector_free(variables_);
                }
#else

                constexpr double residual_tolerance =  1e-4;
                gsl_multiroot_function root_function;

                root_function.n      = active_.size();
                root_function.f      = &multiroot_function_value_cb;
                //root_function.df     = &multiroot_function_jacobian_cb;
                //root_function.fdf    = &multiroot_function_value_and_gradient_cb;
                root_function.params = static_cast<void *>(this);

                gsl_vector *variables_ = gsl_vector_alloc(active_.size());
                for(size_t i = 0; i < active_.size(); ++i) {
                    gsl_vector_set(variables_, i, 1.0);
                }

                // Powell's Hybrid method, but with finite-difference approximation
                const gsl_multiroot_fsolver_type *solver_t_ = gsl_multiroot_fsolver_hybrids;
                gsl_multiroot_fsolver *solver_ = gsl_multiroot_fsolver_alloc(solver_t_, active_.size());
                gsl_multiroot_fsolver_set (solver_, &root_function, variables_);

                int status = GSL_CONTINUE;
                for(size_t iter = 0; iter < max_iterations && GSL_CONTINUE == status; ++iter){
                    status = gsl_multiroot_fsolver_iterate  (solver_);
                    if (GSL_SUCCESS != status){
                        break;
                    }
                    status = gsl_multiroot_test_residual (solver_->f, residual_tolerance);
                }

                std::map<esl::identity<esl::law::property>, double> result_;
                for(size_t i = 0; i < active_.size(); ++i) {
                    result_.emplace(mapping_index_[i], gsl_vector_get(solver_->x, i));
                }

                gsl_multiroot_fsolver_free (solver_);
                gsl_vector_free(variables_);
#endif
                continue;
            }else{
// If Adept is absent, or
#if !defined(ADEPT_VERSION) || !defined(ADEPT_NO_AUTOMATIC_DIFFERENTIATION)
                // TODO: set adaptively
                constexpr double initial_step_size       = 1.0e-5;
                constexpr double line_search_tolerance   = 1.0e-5;
                constexpr double converged_gradient_norm = 1.0e-4;
                constexpr double error_tolerance         = 0.0001;

                // we use the vector Broyden-Fletcher-Goldfarb-Shanno algorithm
                const auto *minimizer_type = gsl_multimin_fdfminimizer_vector_bfgs2;

                gsl_multimin_function_fdf target_;
                target_.n      = active_.size();
                target_.f      = c_minimizer_function_value;
                target_.df     = c_minimizer_function_gradient;
                target_.fdf    = c_minimizer_function_value_and_gradient;
                target_.params = static_cast<void *>(this);

                gsl_vector *x = gsl_vector_alloc(active_.size());
                for(size_t i = 0; i < active_.size(); ++i) {
                    // initial solution is 1.0 * previous quote
                    gsl_vector_set(x, i, 1.0);
                }

                auto *minimizer =
                    gsl_multimin_fdfminimizer_alloc(minimizer_type, active_.size());
                gsl_multimin_fdfminimizer_set(minimizer, &target_, x,
                                              initial_step_size, line_search_tolerance);
                size_t iteration_ = 0;
                int status;
                do {
                    ++iteration_;
                    status = gsl_multimin_fdfminimizer_iterate(minimizer);

                    std::vector<double> solution_;
                    for(size_t param = 0; param < active_.size(); ++param){
                        solution_.push_back(gsl_vector_get (minimizer->x, param));
                    }
                    auto error_ = excess_demand_function_value(&solution_[0]);
                    if(error_ <= error_tolerance){
                        status = GSL_SUCCESS;
                        break;
                    }
                    if(status != GSL_SUCCESS) {
                        break;
                    }
                    status = gsl_multimin_test_gradient(minimizer->gradient,
                                                        converged_gradient_norm);
                } while(GSL_CONTINUE == status && iteration_ < max_iterations);

                if(status == GSL_SUCCESS){
                    std::map<esl::identity<esl::law::property>, double> result_;
                    for(size_t i = 0; i < active_.size(); ++i) {
                        auto scalar_ = gsl_vector_get (minimizer->x, i);
                        result_.insert({mapping_index_[i], scalar_});
                    }
                    gsl_multimin_fdfminimizer_free(minimizer);
                    gsl_vector_free(x);
                    return result_;
                }

                gsl_multimin_fdfminimizer_free(minimizer);
                gsl_vector_free(x);
#else
                LOG(errorlog)  << "gradient-free minimizer failed after " << iteration_
                            << " iterations: " << gsl_strerror(status) << std::endl;
#endif
            }
        }
        return std::nullopt;
    }
}  // namespace tatonnement


