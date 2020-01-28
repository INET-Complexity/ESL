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

#include <stan/model/model_header.hpp>

#include <stan/callbacks/interrupt.hpp>
#include <stan/callbacks/logger.hpp>
#include <stan/callbacks/writer.hpp>

#include <esl/economics/markets/differentiable_demand_supply_function.hpp>
#include <esl/economics/markets/quote.hpp>

namespace tatonnement {

    ///
    ///
    ///
    class excess_demand_model
    : public stan::model::prob_grad
    {
    private:
        std::vector<esl::economics::quote> quotes_;

    public:
        std::vector<std::shared_ptr<differentiable_demand_supply_function>>
            excess_demand_functions_;

        explicit excess_demand_model(
            std::vector<esl::economics::quote> initial_quotes);

        virtual ~excess_demand_model();

        void transform_inits(const stan::io::var_context &context,
                             std::vector<int> &integers,
                             std::vector<double> &reals,
                             std::ostream *stream) const;

        void transform_inits(const stan::io::var_context &context,
                             Eigen::Matrix<double, Eigen::Dynamic, 1> &reals,
                             std::ostream *stream) const;


        template<bool proportional_, bool jacobian_, typename scalar_model_>
        scalar_model_ log_prob(std::vector<scalar_model_> &reals,
                               std::vector<int> &integers,
                               std::ostream *stream = nullptr) const
        {
            (void)stream;
            stan::math::accumulator<scalar_model_> target_;
            stan::io::reader<scalar_model_> reader_(reals, integers);
            scalar_model_ next_scalar_(0.0);
            std::vector<scalar_model_> scalars_;
            scalars_.reserve(quotes_.size());
            for(auto q : quotes_) {
                (void)q;
                if(jacobian_) {
                    scalars_.push_back(reader_.scalar_constrain(next_scalar_));
                } else {
                    scalars_.push_back(reader_.scalar_constrain());
                }
            }

            std::vector<scalar_model_> terms_;
            for(size_t i = 0; i < quotes_.size(); ++i) {
                terms_.emplace_back(scalar_model_(0));
            }

            for(const auto &f : excess_demand_functions_) {
                auto demand_per_property_ = f->excess_demand(quotes_, scalars_);
                if(demand_per_property_.size() != quotes_.size()) {
#if DEBUG
                    // TODO: warning, silently ignoring agents demand
                    continue;
#else
                    throw std::invalid_argument(
                        "agent's excess demand vector does not match the code "
                        "of quoted properties");
#endif
                }

                for(size_t i = 0; i < quotes_.size(); ++i) {
                    terms_[i] += demand_per_property_[i];
                }
            }

            for(auto t : terms_) {
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
    };  // model
}  // namespace tatonnement


struct excess_demand_model_context : public stan::io::var_context
{
    ///
    std::vector<double> quotes;

    ///
    /// \param prices
    explicit excess_demand_model_context(std::vector<double> initial_quotes)
    : stan::io::var_context(), quotes(std::move(initial_quotes))
    {}

    ///
    /// \param name
    /// \return
    [[nodiscard]] bool contains_r(const std::string &name) const override
    {
        return "prices" == name;
    }

    ///
    /// \param name
    /// \return
    [[nodiscard]] std::vector<double>
    vals_r(const std::string &name) const override
    {
        if("prices" == name) {
            return quotes;
        }
        return {};
    }

    ///
    /// \param name
    /// \return
    [[nodiscard]] std::vector<size_t>
    dims_r(const std::string &name) const override
    {
        if("prices" == name) {
            return {quotes.size()};
        }
        return {};
    }

    ///
    /// \param name
    /// \return
    [[nodiscard]] bool contains_i(const std::string &name) const override
    {
        return "assets" == name;
    }

    ///
    /// \param name
    /// \return
    [[nodiscard]] std::vector<int>
    vals_i(const std::string &name) const override
    {
        if("assets" == name) {
            return {static_cast<int>(quotes.size())};
        }
        return std::vector<int>();
    }

    ///
    /// \param name
    /// \return
    [[nodiscard]] std::vector<size_t>
    dims_i(const std::string &name) const override
    {
        if("assets" == name) {
            return {1};
        }
        return {};
    }

    ///
    /// \param names[out]   Real number parameter names
    void names_r(std::vector<std::string> &names) const override
    {
        names = {"prices"};
    }

    ///
    /// \param names[out]   Integer parameter names
    void names_i(std::vector<std::string> &names) const override
    {
        names = {"assets"};
    }
};


struct interrupt_callback
: public stan::callbacks::interrupt
{
    interrupt_callback() = default;

    ///
    /// \brief  Ignore interruptions of the optimisation process
    void operator()() override
    {}
};

///
/// \brief  Stores the states of the variables during the computation. For
/// performance, we may want to consider dropping
///         anything but the last values (states) in release mode as the
///         intermediate values are of no use to the simulation output.
///
class state_writer
: public stan::callbacks::writer
{
public:
    ///
    /// \brief  Captures the states of the parameters during the computation
    ///         process. In debug mode, stores the entire
    ///         history, in release mode only captures the latest values.
    ///
    ///  \warning   Computational processes must emit the optimum values at the
    ///             end, since in release mode intermediate
    ///             values are ignored.
    ///
    std::vector<std::vector<double>> states;

    void operator()(const std::vector<double> &state) override
    {
        states.push_back(state);
    }
};

#endif  // PROJECT_TATONNEMENT_HPP
