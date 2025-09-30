/// \file   vector.hpp
///
/// \brief  Tensor operations specific to vectors
///
/// \authors    Maarten P. Scholl
/// \date       
/// \copyright  Copyright 2025-2025 Maarten P. Scholl
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
#ifndef ESL_MATHEMATICS_VECTOR
#define ESL_MATHEMATICS_VECTOR

namespace esl::mathematics{
    
    // Vector - a 1D tensor with N elements
    template<typename element_t_, std::size_t size_>
    using vector = tensor<element_t_, size_>;


    template<typename element_t_, std::size_t size_>
    constexpr tensor<element_t_, size_, size_> householder_reflection(const tensor<element_t_, size_> &vector)
    {
        auto e = tensor<element_t_, size_>::zeros();
        e(0) = vector.frobenius_norm();

        auto u = vector - e;
        auto norm_u = u.frobenius_norm();
        if(norm_u == element_t_(0)){
            return tensor<element_t_, size_, size_>::identity(); // Return identity if u is zero
        }

        auto v = u / norm_u;

        auto vr = v.expand<0>();
        auto vc = v.expand<1>();

        auto outer_ = vr * vc;

        return tensor<element_t_, size_, size_>::identity() - outer_ * element_t_(2);
    }
}//namespace esl::mathematics

#endif//ESL_MATHEMATICS_VECTOR