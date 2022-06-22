/// \file   zero_truncated_poisson_distribution.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2021-11-03
/// \copyright  Copyright 2017-2021 The Institute for New Economic Thinking,
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
#ifndef MARKET_ECOLOGY_ZERO_TRUNCATED_POISSON_DISTRIBUTION_HPP
#define MARKET_ECOLOGY_ZERO_TRUNCATED_POISSON_DISTRIBUTION_HPP

#include <numeric>
#include <random>


template<typename integer_t_ = int>
class zero_truncated_poisson_distribution
{
    typedef integer_t_ result_type;

    ///
    /// \brief  The mean of the embedded Poisson distribution
    ///
    const double lambda;

    constexpr explicit zero_truncated_poisson_distribution(double lambda)
    : lambda(lambda)
    {

    }

    template<typename uniform_rng_t_>
    result_type
    operator()(uniform_rng_t_ &generator)
    {
        std::uniform_real_distribution distribution_;

        double t = std::exp(-lambda) / (1 - std::exp(-lambda)) * lambda;
        double s = t;

        double u = distribution_(generator);

        result_type k = 1;

        for(; s < u && k < std::numeric_limits<result_type>::max(); ++k){
            t *= (lambda / k);
            s += t;
        }

        return k;
    }
};


#endif //MARKET_ECOLOGY_ZERO_TRUNCATED_POISSON_DISTRIBUTION_HPP
