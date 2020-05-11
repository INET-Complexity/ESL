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

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <vector>
#include <numeric>


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
    }  // namespace rounding

    template<typename iterator_t_>
    auto geometric_mean(iterator_t_ begin, iterator_t_ end)
        -> decltype(typename std::iterator_traits<iterator_t_>::value_type()
                    + 0)
    {
        assert(begin != end);
        auto result_log_ =
            typename std::iterator_traits<iterator_t_>::value_type(0);
        size_t elements_ = 0;
        for(begin; begin != end; ++begin, ++elements_) {
            result_log_ += std::log(*begin);
        }
        return std::exp(result_log_ / elements_);
    }

    template<typename iterator_t_>
    auto growth_rate(iterator_t_ begin, iterator_t_ end)
    -> decltype(typename std::iterator_traits<iterator_t_>::value_type()
                + 0)
    {
        auto result_log_ =
            typename std::iterator_traits<iterator_t_>::value_type(0);
        size_t elements_ = 1;
        auto next_ = std::next(begin);

        for(begin; next_ != end; ++begin, ++next_, ++elements_) {
            result_log_ += std::log(*next_) - std::log(*begin);
        }

        return std::exp(result_log_ / elements_) - 1;
    }

    template<typename container_t_>
    auto geometric_mean(const container_t_ &c)
    {
        return geometric_mean(std::cbegin(c), std::cend(c));
    }

    template<typename floating_t_ = double, typename integer_t_ = size_t>
    std::vector<int>
    allocate_fractions(const std::vector<floating_t_> &fractions, int sum)
    {
        floating_t_ sum_ = 0;
        int near_        = 0;
        std::vector<int> result_;
        for(const auto &f : fractions) {
            sum_ += f;
            near_ += round(f);
            result_.push_back(round(abs(f)));
        }

        int target_ = round(sum_);
        if(near_ == target_) {
            return result_;
        }

        std::vector<size_t> indices_(fractions.size());
        std::iota(indices_.begin(), indices_.end(), 0);
        sort(indices_.begin(),
             indices_.end(),
             [&fractions](size_t i1, size_t i2) {
                 return (fractions[i1] - std::nearbyint(fractions[i1]))
                        > (fractions[i2] - std::nearbyint(fractions[i2]));
             });

        for(size_t i = 0; i < (target_ - near_); ++i) {
            ++result_[indices_[i]];
        }

        return result_;
    }




    template <typename element_t_,
            typename iterator_t_ = decltype(std::begin(std::declval<element_t_>())),
            typename = decltype(std::end(std::declval<element_t_>()))>
    constexpr auto enumerate(element_t_ &&iterable)
    {
        struct iterator
        {
            size_t i;
            iterator_t_ iter;

            bool operator != (const iterator & other) const
            {
                return iter != other.iter;
            }

            void operator ++ ()
            {
                ++i;
                ++iter;
            }

            auto operator * () const
            {
                return std::tie(i, *iter);
            }
        };

        struct iterable_wrapper
        {
            element_t_ iterable;
            auto begin()
            {
                return iterator{ 0, std::begin(iterable) };
            }

            auto end()
            {
                return iterator{ 0, std::end(iterable) };
            }
        };

        return iterable_wrapper{ std::forward<element_t_>(iterable) };
    }

}  // namespace esl
#endif  // ESL_ALGORITHMS_HPP
