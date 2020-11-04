/// \file   test_automatically_differentiable_variable.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-08
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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE variable

#include <boost/test/included/unit_test.hpp>


#include <random>

#include <esl/mathematics/variable.hpp>
using esl::/*mathematics::*/variable;

// TODO: this is a simple implementation of the BH model, and does not use the
//       ESL agents. We may want to include a version like that in our examples.

///
/// \brief  Result structure for the brock hommes function
///
struct simulation_result
{
    std::vector<variable> prices;
    std::vector<variable> n;
    std::vector<variable> U1;
    std::vector<variable> U2;
};

///
/// \param g1
/// \param g2
/// \param b1
/// \param b2
/// \param alpha
/// \param sigma
/// \param R
/// \param w
/// \param beta
/// \param C
/// \param N
/// \return
simulation_result brock_hommes_model( variable g1, variable g2
    , variable b1, variable b2
    , variable alpha
    , variable sigma
    , variable R
    , variable w
    , variable beta
    , variable C
    , size_t N
)
{
    std::vector<variable> prices {1, R};
    std::vector<variable> n {0.5, 0.5};
    std::vector<variable> U1 {0.};
    std::vector<variable> U2 {0.};

    // set up pseudorandom number generators
    std::seed_seq seed_ {0};
    std::default_random_engine generator_(seed_);
    std::uniform_real_distribution<double> distribution_(0.0,1.0 / 1000);

    for(size_t t = 2; t < 2 + N; ++t){
        auto noise = distribution_(generator_);
        auto new_price = ( (n[t-1]  * (g1 * prices[t-1] + b1)) + ((1 - n[t-1]) * (g2 * prices[t-1] + b2)) + noise ) / R;
        prices.emplace_back(new_price);

        // update profits
        //auto m = (1./(alpha * pow(sigma,2))) * (prices[t] - R * prices[t-1]);
        U1.emplace_back((1./(alpha * pow(sigma,2))) * (prices[t] - R * prices[t-1]) * (g1*prices[t-2] + b1 - R * prices[t-1]) + w * U1[t-2] - C);
        U2.emplace_back((1./(alpha * pow(sigma,2))) * (prices[t] - R * prices[t-1]) * (g2*prices[t-2] + b2 - R * prices[t-1]) + w * U2[t-2] - C);

        // population fraction
        n.emplace_back(exp(beta * U1[t-1]) / (exp(beta * U1[t-1]) + exp(beta * U2[t-1])));
    }

    return simulation_result {
        .prices = prices,
        .n      = n,
        .U1     = U1,
        .U2     = U2
    };
}

///
/// \brief quick and dirty implementation of a volatility computation for diff
///         variables
///
/// \param prices
/// \return
variable volatility(const std::vector<variable> &prices)
{
    variable mean_{0};
    for(size_t t = 1; t < prices.size(); ++t){
        mean_ += (adept::exp(adept::log(prices[t-1]) -adept::log(prices[t]) -1) / (prices.size() - 1));
    }

    variable sample_statistic_{0};
    for(size_t t = 1; t < prices.size(); ++t){
        sample_statistic_ += adept::pow(adept::exp(adept::log(prices[t-1]) -adept::log(prices[t])) - 1 - mean_, 2);
    }

    sample_statistic_ /= (prices.size() - 2);

    // to standard deviation
    sample_statistic_ = adept::sqrt(sample_statistic_);

    return sample_statistic_;
}


BOOST_AUTO_TEST_SUITE(ESL)
    BOOST_AUTO_TEST_CASE(brock_hommes_autodiff)
    {
        adept::Stack stack_;

        variable g1     = 0.8;
        variable g2     = 1.1;

        variable b1     = 0.03;
        variable b2     = 0.01;

        variable alpha  = 0.5;
        variable sigma  = 0.2;
        variable R      = 1.01;
        variable w      = 0.5;
        variable beta   = 0.7;
        variable C      = 0.9;

        stack_.new_recording();

        // we don't differentiate for simulation length, this is a regular uint
        size_t N = 10'000;

        // these are 10 differentiable independent variables
        constexpr size_t independents_ = 10;
        stack_.independent(g1);
        stack_.independent(g2);
        stack_.independent(b1);
        stack_.independent(b2);
        stack_.independent(alpha);
        stack_.independent(sigma);
        stack_.independent(R);
        stack_.independent(w);
        stack_.independent(beta);
        stack_.independent(C);

        stack_.set_max_jacobian_threads(2);

        auto result_ = brock_hommes_model
            (  g1
            ,  g2
            ,  b1
            ,  b2
            ,  alpha
            ,  sigma
            ,  R
            ,  w
            ,  beta
            ,  C
            ,  N
            );

        for(size_t i = 0; i < N; ++i){
            // this prints prices
            //std::cout << result_.prices[i] << ", " << std::endl;
        }

        // lets demo with one dependent variable
        auto volatility_ = volatility(result_.prices);

        // this prints annualized volatility
        //std::cout << "volatility " << volatility_.value() * std::sqrt(252) << std::endl;

        stack_.dependent(volatility_);

        //  taken from adept documentation:
        // Compute the Jacobian matrix; note that jacobian_out must be
        // allocated to be of size m*n, where m is the number of dependent
        // variables and n is the number of independents.
        double jacobian_out[independents_ * 1] = {0};

        stack_.jacobian(jacobian_out);

        for(size_t i = 0; i < independents_; ++i){
            BOOST_CHECK(!std::isnan(jacobian_out[i]) && std::isfinite(jacobian_out[i]));
        }
    }


    BOOST_AUTO_TEST_CASE(variable_constructor)
    {
        adept::Stack stack_;
        std::vector<double> v = {1.0, 3.0, 5.0, 7.0};
        for(auto x1 : v){

            esl::variable x = x1;
            stack_.new_recording();

            adept::adouble f = -(x * x * x);

            stack_.independent(&x, 1);
            stack_.dependent(&x, 1);
            double j[1] = {0.};
            stack_.jacobian(j);


            f.set_gradient(1.0);
            stack_.compute_adjoint();
            double dj[1] = {0.};
            adept::get_gradients(&x, 1, dj);

            BOOST_CHECK_CLOSE(adept::value(f), - (adept::value(x) * adept::value(x) * adept::value(x)), 0.000'000'1);

            BOOST_CHECK_CLOSE(dj[0], -3. * (adept::value(x)  * adept::value(x) ) , 0.000'000'1);
        }
    }

BOOST_AUTO_TEST_SUITE_END()  // ESL