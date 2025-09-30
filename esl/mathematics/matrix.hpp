/// \file   matrix.hpp
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
#ifndef ESL_MATHEMATICS_MATRIX
#define ESL_MATHEMATICS_MATRIX

namespace esl::mathematics{

    // Square matrix - a 2D tensor with dimensions N×N
    template<typename element_t_, std::size_t N>
    using square_matrix = tensor<element_t_, N, N>;

    // Matrix - a 2D tensor with dimensions N×M
    template<typename element_t_, std::size_t N, std::size_t M>
    using matrix = tensor<element_t_, N, M>;


}//namespace esl::mathematics

#endif//ESL_MATHEMATICS_MATRIX