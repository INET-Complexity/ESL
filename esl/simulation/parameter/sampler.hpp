/// \file   sampler.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-11-19
/// \copyright  Copyright 2017-2020 The Institute for New Economic Thinking,
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
#ifndef ESL_SAMPLER_HPP
#define ESL_SAMPLER_HPP

#include <vector>
#include <random>

#include <esl/exception.hpp>
#include <esl/simulation/parameter/parameter.hpp>


namespace esl::simulation::parameter {

    namespace sampler {

        ///
        /// \brief  Samples the parameter space in a uniformly random manner
        ///         all values are drawn independently from the provided intervals
        ///         [a,b].
        ///
        /// \tparam numbers_t_      numeric type
        /// \param  parameters      List of parameters to sample
        /// \param  samples         Number of samples to draw
        /// \param seed             Random seed to initialize PseudoRNG
        /// \return
        template<typename number_t_ = double>
        std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>,
                             number_t_>>
        uniform_random(const std::vector<std::shared_ptr<parameter::interval<number_t_>>> &parameters
            , unsigned int samples
            , std::seed_seq seed = std::seed_seq())
        {
            std::mt19937 generator_(seed);

            std::map< std::shared_ptr<parameter::interval<number_t_>>
                , std::uniform_real_distribution<number_t_>
            > distributions_;

            for(const auto &p: parameters){
                // the upper bound is exclusive in the distribution, so we move
                // the bound up
                distributions_.template emplace(p, std::uniform_real_distribution<number_t_>
                    ( p->lower
                    , std::nextafter(p->upper, std::numeric_limits<number_t_>::infinity())));
            }

            std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_>> result_;

            for(unsigned int s = 0; s < samples; ++s){
                std::map<std::shared_ptr<parameter::interval<number_t_>>,
                         number_t_> sample_;
                for(const auto &p: parameters) {
                    sample_[p] = distributions_[p].get()(generator_);
                }
                result_.template emplace(sample_);
            }

            return result_;
        }

        ///
        /// \brief  Samples the parameter space in a grid
        ///
        /// \tparam number_t_
        /// \param parameter_densities
        /// \param seed
        /// \return
        template<typename number_t_ = double>
        std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_>>
        grid(const std::map<std::shared_ptr<parameter::interval<number_t_>>, unsigned int> &parameter_densities)
        {
            std::map<std::shared_ptr<parameter::interval<number_t_>>, std::vector<number_t_> > values_;
            std::map<std::shared_ptr<parameter::interval<number_t_>>, unsigned int > indices_;
            unsigned int points_ = 1;
            for(const auto &[p, d]: parameter_densities){
                points_ *= d;
                values_.emplace(p, std::vector<number_t_>());
                values_[p].reserve(d);
                indices_.emplace(p, 0);
                for(unsigned int s = 0; s < d; ++s){
                    auto v = p->lower + (p->upper - p->lower) / (d - 1) * s;
                    values_[p].template emplace_back(v);
                }
            }
            std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_>> result_;
            for(unsigned int s = 0; s < points_; ++s) {
                unsigned int prefix_ = 1;
                indices_.clear();
                for(const auto &[p, d] : parameter_densities) {
                    indices_.template emplace(p,  (s / prefix_) % d);
                    prefix_ *= d;
                }
                std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_> sample_;
                for(const auto &[p, i] :indices_) {
                    sample_[p] = values_.find(p)->second[i];
                }
                result_.template emplace_back(sample_);
            }
            return result_;
        }

        ///
        /// \brief
        ///
        /// \tparam number_t_
        /// \param parameter_densities
        /// \param seed
        /// \return
        template<typename number_t_ = double>
        std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_>>
        latin_hypercube(const std::vector<std::shared_ptr<parameter::interval<number_t_>>> &parameters, unsigned int density, std::seed_seq seed = std::seed_seq())
        {
            std::mt19937_64 generator_(seed);

            std::map<std::shared_ptr<parameter::interval<number_t_>>, std::vector<number_t_>> values_;
            std::map< std::shared_ptr<parameter::interval<number_t_>>, std::vector<size_t>> indices_;

            unsigned int points_ = std::min(density, 1u) ;
            for(const auto &p: parameters){
                points_ *= density;
                values_.emplace(p, std::vector<number_t_>());
                indices_.emplace(p, std::vector<size_t>());
                values_[p].reserve(density);
                indices_[p].reserve(density);
                for(unsigned int s = 0; s < density; ++s){
                    auto v = p->lower + (p->upper - p->lower) / (density - 1) * s;
                    auto i = values_[p].template emplace_back(v);
                    indices_[p].template emplace_back(s);
                }
                std::shuffle(indices_[p].begin(), indices_[p].end(), generator_);
            }
            std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_>> result_;

            for(unsigned int s = 0; s < density; ++s) {
                typename decltype(result_)::value_type sample_;
                for(const auto &[p, index_]: indices_){
                    sample_[p] = values_[p][index_[s]];
                }
                result_.template emplace_back(sample_);
            }

            return result_;
        }

        ///
        /// \brief
        ///
        /// \tparam number_t_
        /// \param parameter_densities
        /// \param seed
        /// \return
        template<typename number_t_ = double>
        std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_>>
        orthogonal(std::shared_ptr<parameter::interval<number_t_>> &parameters, unsigned int density, std::seed_seq seed = std::seed_seq())
        {
            std::mt19937_64 generator_(seed);

            std::map<std::shared_ptr<parameter::interval<number_t_>>, std::vector<number_t_>> values_;
            std::map< std::shared_ptr<parameter::interval<number_t_>>, std::vector<size_t>> indices_;

            unsigned int points_ = std::min(density, 1u) ;
            for(const auto &p: parameters){
                points_ *= density;
                values_.emplace(p, std::vector<number_t_>());
                indices_.emplace(p, std::vector<size_t>());
                values_[p].reserve(density);
                indices_[p].reserve(density);
                for(unsigned int s = 0; s < density; ++s){
                    auto v = p->lower + (p->upper - p->lower) / (density - 1) * s;
                    auto i = values_[p].template emplace_back(v);
                    indices_[p].template emplace_back(s);
                }
                std::shuffle(indices_[p].begin(), indices_[p].end(), generator_);
            }
            std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_>> result_;
            for(unsigned int s = 0; s < points_; ++s) {
                typename decltype(result_)::value_type sample_;
                for(const auto &[p, index_]: indices_){
                    sample_[p] = values_[p][index_[s]];
                }
                result_.template emplace_back(sample_);
            }

            return result_;
        }
    }
////////////////////////////////////////////////////////////////////////////////
}

#endif  // ESL_SAMPLER_HPP
