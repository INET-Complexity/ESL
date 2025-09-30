/// \file   bernoulli.hpp
///
/// \brief  Tensor operations specific to matrices, and matrix-vector .
///
/// \authors    Maarten P. Scholl
/// \date       
/// \copyright  Copyright 2025- Maarten P. Scholl
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
#ifndef ESL_MATHEMATICS_BERNOULLI
#define ESL_MATHEMATICS_BERNOULLI

#include <cstddef>
#include <iostream>
#include <type_traits>
#include <ratio>

namespace esl::mathematics{

    /// 
    /// \brief Helper struct for factorial calculations at compile time
    ///
    template<std::size_t N>
    struct factorial
    {
        static constexpr std::size_t value = N * factorial<N - 1>::value;
    };

    template<>
    struct factorial<0>
    {
        static constexpr std::size_t value = 1;
    };

    ///
    /// \brief Helper for binomial coefficient calculation at compile time
    ///
    template<std::size_t N, std::size_t K>
    struct binomial
    {
        static constexpr std::size_t value = (factorial<N>::value / (factorial<K>::value * factorial<N - K>::value));
    };

    ///
    /// \brief Bernoulli numbers implementation using std::ratio
    /// Uses the recursive formula to compute Bernoulli numbers at compile time
    ///
    template<std::size_t N>
    struct bernoulli_number_impl
    {
    private:
        // Implements the recursion: sum(k=0 to n-1) binomial(n, k) * B_k = 0
        template<std::size_t K, typename Sum = std::ratio<0>>
        struct sum_helper
        {
            using b_k = typename bernoulli_number_impl<K>::type;
            using binomial_term = std::ratio<binomial<N, K>::value, 1>;
            using product = std::ratio_multiply<binomial_term, b_k>;
            using new_sum = std::ratio_add<Sum, product>;

            using type = typename sum_helper<K + 1, new_sum>::type;
        };

        template<typename sum>
        struct sum_helper<N, sum>
        {
            using type = sum;
        };

        using sum_result = typename sum_helper<0, std::ratio<0>>::type;

        static constexpr std::intmax_t binomial_n_n = binomial<N, N>::value;

        using negated_sum = std::ratio<-sum_result::num, sum_result::den>;

        using result = std::ratio<negated_sum::num, negated_sum::den *binomial_n_n>;

    public:
        using type = result;
    };

    // Special case for B_0 = 1
    template<>
    struct bernoulli_number_impl<0>
    {
        using type = std::ratio<1>;
    };

    // Special case for B_1 = -1/2
    template<>
    struct bernoulli_number_impl<1>
    {
        using type = std::ratio<-1, 2>;
    };

    //// Odd Bernoulli numbers (except B_1) are zero
    //template<std::size_t N>
    //struct bernoulli_number
    //: std::conditional_t<(N > 1 && N % 2 == 1), std::type_identity<std::ratio<0>>, bernoulli_number_impl<N>>::type
    //{

    //};

    template<std::size_t N>
    using bernoulli_number = std::conditional_t<(N > 1 && N % 2 == 1), std::type_identity<std::ratio<0>>, bernoulli_number_impl<N>>::type;

     

}//namespace esl::mathematics

#endif//ESL_MATHEMATICS_BERNOULLI