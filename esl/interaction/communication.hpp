/// \file   communication.hpp
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
#ifndef MARKET_ECOLOGY_COMMUNICATION_HPP
#define MARKET_ECOLOGY_COMMUNICATION_HPP

#include <random>

#include <esl/simulation/time.hpp>
#include <cassert>

namespace esl {
    template<typename t>
    struct identity;

    class agent;
}

namespace esl::interaction::communication {

    class latency_model
    {
    private:

    public:
        latency_model() = default;

        virtual ~latency_model() = default;

        virtual simulation::time_duration
        sample(const identity<agent> &sender, const identity<agent> &recipient, std::seed_seq &seed) = 0;
    };


    class instant
        : public latency_model
    {
        using latency_model::latency_model;

        virtual simulation::time_duration
        sample(const identity<agent> &sender, const identity<agent> &recipient,
               std::seed_seq &seed)
        {
            (void)sender;
            (void)recipient;
            (void)seed;
            return simulation::time_duration(0);
        }
    };



    ///
    /// \brief  In this model, the latency is given by minimum cost `shift` (e.g., the physical distance between the
    ///         agents over the speed of light in fibre optic cable),
    ///         plus the waiting time for `shape` independent tasks that have random waiting time ~ exponential(rate).
    ///
    class shifted_gamma_latency
    : public latency_model
    {
    private:
    public:
        double shape;
        double rate;
        double shift;

        constexpr shifted_gamma_latency(double shape, double rate, double shift = 0.)
        : shape(shape)
        , rate(rate)
        , shift(shift)
        {
            assert(shape > 0);
        }

        simulation::time_duration sample( const identity<agent> &sender
                                        , const identity<agent> &recipient
                                        , std::seed_seq &seed) override
        {
            (void)sender;
            (void)recipient;
            auto gamma_ = std::gamma_distribution<double>(shape, rate);
            std::mt19937 gen(seed);
            auto real_ = gamma_(gen);
            return simulation::time_duration( shift + real_);
        }
    };

}//namespace esl::interaction

#endif //MARKET_ECOLOGY_COMMUNICATION_HPP
