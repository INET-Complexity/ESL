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
                    values_[p].emplace_back(v);
                }
            }
            std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_>> result_;
            for(unsigned int s = 0; s < points_; ++s) {
                unsigned int prefix_ = 1;
                indices_.clear();
                for(const auto &[p, d] : parameter_densities) {
                    indices_. emplace(p,  (s / prefix_) % d);
                    prefix_ *= d;
                }
                std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_> sample_;
                for(const auto &[p, i] :indices_) {
                    sample_[p] = values_.find(p)->second[i];
                }
                result_. emplace_back(sample_);
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
                    auto i = values_[p]. emplace_back(v);
                    indices_[p]. emplace_back(s);
                }
                std::shuffle(indices_[p].begin(), indices_[p].end(), generator_);
            }
            std::vector<std::map<std::shared_ptr<parameter::interval<number_t_>>, number_t_>> result_;

            for(unsigned int s = 0; s < density; ++s) {
                typename decltype(result_)::value_type sample_;
                for(const auto &[p, index_]: indices_){
                    sample_[p] = values_[p][index_[s]];
                }
                result_. emplace_back(sample_);
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

        /*constexpr */ unsigned int get_best_distance( unsigned int dimensions
            , unsigned int samples
            , unsigned int duplication
            , const std::vector<std::vector<unsigned int>> &result
            , const std::vector<unsigned int> &point
            , unsigned int count)
        {
            const auto adjustment_ = std::pow<double>(samples,1 - 1.0 / dimensions );

            auto min_all = std::numeric_limits<double>::max() ;
            unsigned int best = 0;

            for (unsigned int  k = 0; k < duplication * count; ++k){
                auto min_can = std::numeric_limits<double>::max();

                for (unsigned int j = count; j < samples; ++j){
                    auto distance_ = 0.0;
                    for (unsigned int i = 0; i < dimensions; ++i){
                        distance_ +=  ( point[i+k* dimensions] - result[i][j] )
                                      * ( point[i+k* dimensions] - result[i][j] );
                    }
                    if (distance_ < min_can ){
                        min_can = distance_;
                    }
                }
                min_can = fabs (  sqrt ( min_can ) - adjustment_);
                if (min_can < min_all ){
                    min_all =min_can;
                    best = k;
                }

            }

            return best;
        }

///
/// \brief
///
/// \details    Brian Beachkofski, Ramana Grandhi,
///             Improved Distributed Hypercube Sampling,
///             American Institute of Aeronautics and Astronautics Paper
///             2002-1274.
///
/// \param dimensions
/// \param samples
/// \param seed
/// \param duplication  Hyperparameter, default = 5, the larger the number
///                     the better the result but the more costly the
///                     computational cost which is linearly dependent on this
///                     parameter
/// \return for each dimension,
        std::vector<std::vector<unsigned int>>
        improved_hypercube_sampling( unsigned int dimensions
            , unsigned int samples
            , std::seed_seq &seed
            , unsigned int duplication = 5
        )
        {
            assert(samples > 0);
            auto result_ = std::vector<std::vector<unsigned int>>(dimensions, std::vector<unsigned int>(samples, 0));

            std::mt19937_64 generator_(seed);
            {
                std::uniform_int_distribution<unsigned int> distribution_(1, samples);
                for(unsigned int i = 0; i < dimensions; i++) {
                    result_[i][samples - 1] = distribution_(generator_);
                }
            }

            typedef std::tuple<unsigned int,unsigned int> key_t_;

            struct key_hash
                : public std::function<size_t(key_t_)>
            {
                std::size_t operator()(const key_t_ &k) const
                {
                    return (std::get<0>(k) << 16) | std::get<1>(k);
                }
            };

            std::unordered_map<key_t_, std::tuple<unsigned int,unsigned int>, key_hash> reverse_;

            // holds points that can still be selected to add to the set of valid points
            auto options_ = std::vector<std::vector<unsigned int>>(dimensions, std::vector<unsigned int>(samples,0));

            for(unsigned int j = 0; j < samples; ++j){
                for(unsigned int  i = 0; i < dimensions; ++i){
                    options_[i][j] = j + 1;
                    reverse_.insert({{i,j + 1}, {i,j}});
                }
            }

            for (unsigned int i = 0; i < dimensions; ++i){
                auto j = (result_[i][samples - 1] - 1);
                options_[i][j] = samples;
                reverse_[{i, samples}] = {i,j};
            }

            auto point = std::vector<unsigned int>(dimensions * duplication * samples, 0);

            for(unsigned int count = samples - 1; 2 <= count; count--) {

                auto list  = std::vector<unsigned int>(duplication * samples, 0);

                for(unsigned int i = 0; i < dimensions; ++i) {

                    for(unsigned int k = 0; k < duplication; ++k) {
                        for(unsigned int j = 0; j < count; ++j) {
                            list[j + k * count] = options_[i][j];
                        }
                    }

                    for(unsigned int k = count * duplication; 0 < k; --k) {
                        std::uniform_int_distribution<unsigned int> distribution_(0, k-1);

                        unsigned int point_index               = distribution_(generator_);
                        point[i + (k-1) * dimensions] = list[point_index];
                        list[point_index]         = list[k-1];
                    }
                }

                auto best = get_best_distance(dimensions, samples, duplication, result_, point, count);

                for (unsigned int i = 0; i < dimensions; ++i){
                    result_[i][(count-1)] = point[i+best* dimensions];
                }

                for (unsigned int i = 0; i < dimensions; ++i){
                    auto iterator_ = reverse_.find({i, result_[i][(count-1)]});
                    if(reverse_.end() == iterator_){
                        throw std::exception();
                    }
                    if (options_[i][std::get<1>(iterator_->second)] != result_[i][(count-1)] ) {
                        throw std::exception();
                    }

                    auto j = std::get<1>(iterator_->second);

                    auto v = options_[i][(count-1)];
                    options_[i][j] = v;
                    reverse_[{i, v}] = {i, j};
                }
            }
            for (unsigned int  i = 0; i < dimensions; ++i){
                result_[i][0] = options_[i][0];
            }
            return result_;
        }

    }
////////////////////////////////////////////////////////////////////////////////
}

#endif  // ESL_SAMPLER_HPP
