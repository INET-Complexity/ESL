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

#include <gsl/gsl_vector.h>

#include <esl/economics/markets/differentiable_demand_supply_function.hpp>
#include <esl/economics/markets/quote.hpp>




namespace tatonnement {

    ///
    /// \brief
    ///
    class excess_demand_model
    {
    public:
        std::vector<std::shared_ptr<differentiable_demand_supply_function>>
            excess_demand_functions_;

        explicit excess_demand_model(
            std::map<esl::identity<esl::law::property>, esl::economics::quote> initial_quotes);

        virtual ~excess_demand_model();


    protected:
        std::map<esl::identity<esl::law::property>, esl::economics::quote> quotes_;

        adept::Stack stack_;                    // Adept stack object
        std::vector<adept::adouble> active_x_;  // Active state variables


        adept::adouble calc_function_value(const adept::adouble *x);
        double         calc_function_value(const double *x);

        double calc_function_value_and_gradient(const double *x, double *dJ_dx) ;

        static double my_function_value(const gsl_vector *variables, void *params);
        static void my_function_gradient(const gsl_vector *x, void *params, gsl_vector *gradJ);
        static void my_function_value_and_gradient(const gsl_vector *x, void *params, double *J, gsl_vector *gradJ);




    public:

        std::optional<std::map<esl::identity<esl::law::property>, double>> do_compute();









/*
        template<bool proportional_, bool jacobian_, typename scalar_model_>
        scalar_model_ log_prob(std::vector<scalar_model_> &reals,
                               std::vector<int> &integers,
                               std::ostream *stream = nullptr) const
        {
            (void)stream;
            stan::math::accumulator<scalar_model_> target_;
            stan::io::reader<scalar_model_> reader_(reals, integers);
            scalar_model_ next_scalar_(0.0);

            std::map< esl::identity<esl::law::property>
                                  , std::tuple<esl::economics::quote, scalar_model_>
                                  > qv_ed_;

            std::vector<scalar_model_> scalars_;
            scalars_.reserve(quotes_.size());
            for(auto [k,v] : quotes_) {

                if(jacobian_) {
                    auto n = reader_.scalar_constrain(next_scalar_);
                    scalars_.push_back(n);
                    qv_ed_.insert({k, std::make_tuple(v, n)});
                } else {
                    auto n = reader_.scalar_constrain();
                    scalars_.push_back(n);
                    qv_ed_.insert({k, std::make_tuple(v, n)});
                }
            }

            //std::vector<scalar_model_> terms_;
            std::map< esl::identity<esl::law::property>, scalar_model_> terms_map;
            //for(size_t i = 0; i < quotes_.size(); ++i) {
            //    terms_.emplace_back(scalar_model_(0));
            //}

            for(auto [k,v]: quotes_) {
                (void) v;
                terms_map.insert({k, scalar_model_(0)});
            }

            for(const auto &f : excess_demand_functions_) {
                auto demand_per_property_ = f->excess_demand_m(qv_ed_);
                if(demand_per_property_.size() != quotes_.size()) {
//#if DEBUG
                    // TODO: warning, silently ignoring agents demand
                    continue;
//#else
//                    throw std::invalid_argument(
//                        "agent's excess demand vector does not match the code "
//                        "of quoted properties");
//#endif
                }

                for(auto [k,v]: demand_per_property_) {
                    terms_map[k] += v;
                }

                //for(size_t i = 0; i < quotes_.size(); ++i) {
                //    terms_[i] += demand_per_property_[i];
                //}
            }

            for(auto [p,t] : terms_map) {
                (void) p;
                target_.add(-pow(t, 2));
            }

            target_.add(next_scalar_);

            auto sum_ = target_.sum();
            return sum_;
        }

        ///
        /// \brief
        ///
        /// \tparam propto
        /// \tparam jacobian
        /// \tparam scalar_model_
        /// \param params_r
        /// \param stream
        /// \return
        template<bool proportional_, bool jacobian_, typename scalar_model_>
        scalar_model_
            log_prob(Eigen::Matrix<scalar_model_, Eigen::Dynamic, 1> &reals,
                     std::ostream *stream = nullptr) const
        {
            return log_prob<proportional_, jacobian_, scalar_model_>(
                std::vector<scalar_model_>(reals.data(),
                                           reals.data() + reals.size()),
                std::vector<int>(), stream);
        }

        /// \param names[out]   Is set to the names of the parameters to the
        /// model
        void get_param_names(std::vector<std::string> &names) const;

        /// \param dimensions[out]  The dimensions of each parameter.
        void get_dims(std::vector<std::vector<size_t>> &dimensions) const;

        template<typename random_number_generator_t_>
        void write_array(random_number_generator_t_ &generator,
                         std::vector<double> &reals, std::vector<int> &integers,
                         std::vector<double> &variables,
                         bool transformed_parameters = true,
                         bool generated_quantities   = true,
                         std::ostream *stream        = nullptr) const
        {
            (void)generator;
            (void)transformed_parameters;
            (void)generated_quantities;
            (void)stream;

            stan::io::reader<double> in_(reals, integers);

            variables.clear();
            variables.reserve(quotes_.size());
            for(auto q : quotes_) {
                (void)q;
                variables.push_back(in_.scalar_constrain());
            }
        }

        template<typename random_number_generator_t_>
        void write_array(random_number_generator_t_ &generator,
                         Eigen::Matrix<double, Eigen::Dynamic, 1> &reals,
                         Eigen::Matrix<double, Eigen::Dynamic, 1> &variables,
                         bool transformed_parameters = true,
                         bool generated_quantities   = true,
                         std::ostream *stream        = nullptr) const
        {
            (void)variables;
            auto params_r_vec =
                std::vector<double>(reals.data(), reals.data() + reals.size());

            std::vector<int> params_i_vec;
            std::vector<double> vars_vec;
            write_array(generator, params_r_vec, params_i_vec, vars_vec,
                        transformed_parameters, generated_quantities, stream);

            reals = Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, 1>>(
                vars_vec.data(), vars_vec.size(), 1);
        }

        void constrained_param_names(std::vector<std::string> &names,
                                     bool transformed_parameters = true,
                                     bool generated_quantities   = true) const;

        void unconstrained_param_names(std::vector<std::string> &names,
                                       bool transformed_parameters = true,
                                       bool generated_quantities = true) const;

                                       */


    };  // model
}  // namespace tatonnement

#endif  // PROJECT_TATONNEMENT_HPP
