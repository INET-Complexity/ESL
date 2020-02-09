/// \file   algorithms.hpp
///
/// \brief  General purpose algorithms and datastructures
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-30
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
#ifndef ESL_ALGORITHMS_HPP
#define ESL_ALGORITHMS_HPP

#include <array>
#include <cmath>
#include <iterator>

namespace esl {

    ///
    /// \brief Copy container elements into a std::array
    ///
    /// \tparam element_t_
    /// \tparam begin_
    /// \tparam end_
    /// \tparam container_t_
    /// \param collection
    /// \return
    template<size_t begin_,
             size_t end_,
             typename element_t_,
             class container_t_>
    std::array<element_t_, size_t(end_ - begin_)>
    to_array(const container_t_ &collection)
    {
        static_assert(end_ >= begin_);
        assert(collection.size() >= end_);
        std::array<element_t_, size_t(end_ - begin_)> result_;
        auto first_ = collection.begin();
        std::advance(first_, begin_);
        auto second_ = collection.begin();
        std::advance(second_, end_);
        std::copy(first_, second_, result_.data());
        return result_;
    }

    ///
    /// \brief  Copies the contents of multiples std::array into a new sequence
    ///
    ///
    /// \tparam element_t_
    /// \tparam first_
    /// \tparam second_
    /// \param a
    /// \param b
    /// \return
    template<typename element_t_, size_t first_, size_t second_>
    constexpr inline std::array<element_t_, size_t(first_ + second_)>
    array_concatenate(const std::array<element_t_, first_> &a,
                const std::array<element_t_, second_> &b)
    {
        std::array<element_t_, first_ + second_> result_ = {};
        size_t i                                         = 0;
        for(; i < first_; ++i) {
            result_[i] = a[i];
        }
        for(; i < first_ + second_; ++i) {
            result_[i] = b[i - first_];
        }
        return result_;
    }

    ///
    /// \brief  Slices a std::array
    ///
    /// \tparam start_
    /// \tparam end_
    /// \tparam element_t_
    /// \tparam original_s_
    /// \param a
    /// \return
    template<size_t start_,
             size_t end_,
             typename element_t_,
             size_t original_s_>
    constexpr inline std::array<element_t_, (end_ - start_)>
    array_slice(const std::array<element_t_, original_s_> &a)
    {
        static_assert(end_ <= original_s_);
        std::array<element_t_, (end_ - start_)> result_ = {};
        for(size_t i = start_; i < end_; ++i) {
            result_[i - start_] = a[i];
        }
        return result_;
    }



    namespace rounding {


        template<typename floating_t_, typename integer_t_ = int64_t>
        integer_t_ integer_towards_zero(floating_t_ value)
        {
            return integer_t_(std::trunc(value));
        }

        template<typename floating_t_, typename integer_t_ = int64_t>
        integer_t_ integer_away_from_zero(floating_t_ value)
        {
            return integer_t_(std::trunc(value) + sign(value));
        }

    }//namespace rounding


    template<typename iterator_t_>
    auto geometric_mean(iterator_t_ begin, iterator_t_ end) -> decltype( typename std::iterator_traits<iterator_t_>::value_type() + 0 )
    {
        assert(begin != end);
        auto result_log_ = typename std::iterator_traits<iterator_t_>::value_type(0);
        size_t elements_ = 0;
        for(begin; begin != end; ++begin, ++elements_){
            result_log_ += std::log(*begin);
        }
        return std::exp(result_log_ / elements_);
    }

    template<typename container_t_>
    auto geometric_mean(const container_t_ &c)
    {
        return geometric_mean(std::cbegin(c), std::cend(c));
    }


}  // namespace esl

#endif  // ESL_ALGORITHMS_HPP
