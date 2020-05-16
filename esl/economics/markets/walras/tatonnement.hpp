/// \file   tatonnement.hpp
///
/// \brief  Implements the tâtonnement process (hill climbing), implemented as a
///         numerical optimisation (L-BFGS) with
///         automatic differentiation using the Stan-math library.
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

#include <adept.h>

#include <esl/economics/markets/differentiable_demand_supply_function.hpp>
#include <esl/economics/markets/quote.hpp>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

///
/// The following functions defined callbacks for various optimisers used to
/// solve the tatonnement problem. These functions reside in the
/// global namespace, because external "C" linkage doesn't deal with namespaces.
///

extern "C" double my_function_value(const gsl_vector *variables, void *params);
extern "C" void my_function_gradient(const gsl_vector *x, void *params, gsl_vector *gradJ);
extern "C" void my_function_value_and_gradient(const gsl_vector *x, void *params, double *J, gsl_vector *gradJ);

extern "C" int multiroot_function_value_cb(const gsl_vector *x, void *params, gsl_vector *f);
extern "C" int multiroot_function_jacobian_cb(const gsl_vector * x, void * params, gsl_matrix * df);
extern "C" int multiroot_function_value_and_gradient_cb(const gsl_vector * x, void * params, gsl_vector * f, gsl_matrix *df);

namespace esl::economics::markets::tatonnement {
    ///
    /// \brief
    ///
    class excess_demand_model
    {
    public:
        std::vector<std::shared_ptr<differentiable_demand_supply_function>> excess_demand_functions_;

        explicit excess_demand_model(std::map<identity<law::property>, quote> initial_quotes);

        virtual ~excess_demand_model();

        ///
        /// \brief
        ///
        enum solver { minimization
                    , multiple_root
        };

        ///
        /// \brief  The default approach is to try the root-finding approach
        ///         first, followed by the minimization approach if the
        ///         root finding solver does not make progress towards a
        ///         solution.
        ///
        std::vector<solver> methods = {multiple_root/*, minimization*/};

    protected:
        std::map<identity<law::property>, quote> quotes_;

        adept::Stack stack_;
        std::vector<adept::adouble> active_;

        adept::adouble calc_function_value(const adept::adouble *x);
        std::vector<adept::adouble> multiroot_function_value(const adept::adouble *x);

#if !defined(ADEPT_VERSION) | !defined(ADEPT_NO_AUTOMATIC_DIFFERENTIATION)
        double         calc_function_value(const double *x);
        std::vector<double> multiroot_function_value(const double *x);


        double minimizer_function_value_and_gradient(const double *x, double *dJ_dx) ;
        std::vector<double> multiroot_function_value_and_gradient(const double *x, double *dJ_dx) ;
#endif

        friend double ::my_function_value(const gsl_vector *variables, void *params);


#if !defined(ADEPT_VERSION) | !defined(ADEPT_NO_AUTOMATIC_DIFFERENTIATION)
        friend void ::my_function_gradient(const gsl_vector *x, void *params, gsl_vector *gradJ);
        friend void ::my_function_value_and_gradient(const gsl_vector *x, void *params, double *J, gsl_vector *gradJ);
#endif

        friend int ::multiroot_function_value_cb(const gsl_vector *x, void *params, gsl_vector *f);

#if !defined(ADEPT_VERSION) | !defined(ADEPT_NO_AUTOMATIC_DIFFERENTIATION)
        friend int ::multiroot_function_jacobian_cb(const gsl_vector * x, void * params, gsl_matrix * df);
        friend int ::multiroot_function_value_and_gradient_cb(const gsl_vector * x, void * params, gsl_vector * f, gsl_matrix *df);
#endif

    public:
        std::optional<std::map<identity<law::property>, double>>
        compute_clearing_quotes();
    };  // model
}  // namespace tatonnement

#endif  // PROJECT_TATONNEMENT_HPP
