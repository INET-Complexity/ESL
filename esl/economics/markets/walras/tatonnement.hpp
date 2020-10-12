/// \file   tatonnement.hpp
///
/// \brief  Implements the tâtonnement process (hill climbing), implemented as a
///         numerical optimisation using quasi-Newton methods.
///
/// \remark This code uses the spelling `tatonnement`, as the accent on `â` can
///         not be rendered in some filesystem character sets.
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
#ifndef PROJECT_TATONNEMENT_HPP
#define PROJECT_TATONNEMENT_HPP

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <adept.h>

#include <esl/economics/markets/differentiable_demand_supply_function.hpp>
#include <esl/economics/markets/quote.hpp>
#include <esl/economics/markets/walras/differentiable_order_message.hpp>


#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <esl/law/property_collection.hpp>

///
/// The following functions defined callbacks for various optimisers used to
/// solve the tatonnement problem. These functions reside in the
/// global namespace, because external "C" linkage doesn't deal with namespaces.
///

extern "C" double c_minimizer_function_value(const gsl_vector *variables, void *params);
extern "C" void c_minimizer_function_gradient(const gsl_vector *x, void *params, gsl_vector *gradJ);
extern "C" void c_minimizer_function_value_and_gradient(const gsl_vector *x, void *params, double *J, gsl_vector *gradJ);

extern "C" int multiroot_function_value_cb(const gsl_vector *x, void *params, gsl_vector *f);
extern "C" int multiroot_function_jacobian_cb(const gsl_vector * x, void * params, gsl_matrix * df);
extern "C" int multiroot_function_value_and_gradient_cb(const gsl_vector * x, void * params, gsl_vector * f, gsl_matrix *df);

extern "C" double uniroot_function_value (double x, void *params);
extern "C" double uniroot_function_value_and_gradient (double x, void *params);
extern "C" void   uniroot_function_jacobian_cb (double x, void *parameters, double *y, double *dy);



namespace esl::economics::markets::tatonnement {
    ///
    /// \brief
    ///
    class excess_demand_model
    {
    public:
        std::vector<std::shared_ptr<walras::differentiable_order_message>> excess_demand_functions_;

        explicit excess_demand_model(law::property_map<quote> initial_quotes);

        virtual ~excess_demand_model();

        ///
        /// \brief
        ///
        enum solver { minimization
                    , root
        };

        ///
        /// \brief  Limit maximum price decrease/increase
        ///
        std::pair<double, double> circuit_breaker = {1./1.1, 1.1};

        ///
        /// \brief  The default approach is to try the root-finding approach
        ///         first, followed by the minimization approach if the
        ///         root finding solver does not make progress towards a
        ///         solution.
        ///
        std::vector<solver> methods = { root
                                      , minimization};

        law::property_map<quote> quotes;

    protected:
        ///
        /// \brief  Adept data structure to track expressions
        ///
        adept::Stack stack_;

        ///
        /// \brief  Currently active differentiable variables. Stored in one place on the model, so that we can use
        ///         external solvers that operate on a pointer or reference to these variables
        ///
        std::vector<adept::adouble> active_;

        ///
        /// \brief
        ///
        /// \param multipliers
        /// \return
        adept::adouble demand_supply_mismatch(const adept::adouble *multipliers);

        ///
        /// \brief
        ///
        /// \param multipliers
        /// \return
        std::vector<adept::adouble> excess_demand(const adept::adouble *multipliers);

        double excess_demand_function_value(const double *multipliers);

        double minimizer_function_value_and_gradient(const double *multipliers, double *derivatives) ;

        friend void ::c_minimizer_function_gradient(const gsl_vector *multipliers, void *model, gsl_vector *derivatives);
        friend void ::c_minimizer_function_value_and_gradient(const gsl_vector *x, void *params, double *J, gsl_vector *gradJ);
        friend double ::c_minimizer_function_value(const gsl_vector *variables, void *params);

        friend int ::multiroot_function_jacobian_cb(const gsl_vector *multipliers, void * params, gsl_matrix *derivatives);
        friend int ::multiroot_function_value_and_gradient_cb(const gsl_vector *multipliers, void * params, gsl_vector * f, gsl_matrix *derivatives);
        std::vector<double> multiroot_function_value_and_gradient(const double *multipliers, double *derivatives) ;

        std::vector<double> multiroot_function_value(const double *multipliers);
        friend int ::multiroot_function_value_cb(const gsl_vector *x, void *params, gsl_vector *f);

        friend double ::uniroot_function_value (double x, void *params);
        friend double ::uniroot_function_value_and_gradient (double x, void *params);
        friend void   ::uniroot_function_jacobian_cb (double x, void *parameters, double *y, double *dy);

    public:
        std::optional<std::map<identity<law::property>, double>>
        compute_clearing_quotes(size_t max_iterations = 1000);
    };  // model
}  // namespace tatonnement

#endif  // PROJECT_TATONNEMENT_HPP
