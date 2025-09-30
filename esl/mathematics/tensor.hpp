/// \file   
///
/// \brief  
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
#ifndef ESL_MATHEMATICS_TENSOR
#define ESL_MATHEMATICS_TENSOR

#include <array>
#include <type_traits>
#include <algorithm>
#include <numeric>

#undef min
#undef max

#include <esl/mathematics/tuple.hpp>
#include <esl/mathematics/range.hpp>


namespace esl::mathematics{
    /// 
    /// \brief  A tensor class
    /// 
    /// \detail Default alignment is multiples of the size of the tensor's storage
    /// 
    template<typename element_t_, std::size_t... dimensions>
    struct 
    alignas(alignof(std::array<element_t_, (... * dimensions)>))
    tensor
    {
        /// 
        /// \brief  Dependent type name for the elements of the tensor
        /// 
        using element_type = element_t_;

        /// 
        /// \brief  All dimensions must be greater than zero - any zero would make the tensor degenerate, 
        ///         and the underlying storage doesn't allow it. 
        /// 
        /// \detail We use this assertion to capture non-obvious cases, e.g. where some compile-time type computation results in a degenerate tensor.
        /// 
        static_assert((sizeof...(dimensions) > 0), "Tensor must have at least one dimension");
        static_assert((... && (dimensions > 0)), "All tensor dimensions must be greater than 0");

        /// 
        /// \brief  The shape of the tensor as a tuple
        /// 
        using shape = std::tuple<std::integral_constant<std::size_t, dimensions>...>;

        /// 
        /// \brief  The rank of the tensor, meaning the number of dimensions
        /// 
        static constexpr std::size_t rank = sizeof ... (dimensions);

        /// 
        /// \brief  The size of the tensor is the total number of elements across all dimensions
        /// 
        static constexpr std::size_t size = (... * dimensions);

        /// 
        /// \brief  The size of the diagonal of the tensor
        /// 
        static constexpr std::size_t diagonal_size = std::min({ dimensions... });



        /// 
        /// \brief  Function template to convert between a tuple with sizes for each dimension, and a tensor with that dimensionality, at the type level.
        /// 
        template<typename tuple_t_, std::size_t... indices>
        static constexpr auto tuple_to_tensor(tuple_t_, std::index_sequence<indices...>) -> tensor<element_t_, std::tuple_element_t<indices, tuple_t_>::type::value...>;

        /// 
        /// \brief  Computes the type of the transpose of the tensor. Uses code from mathematics/tuple.hpp
        /// 
        using transposed_type = decltype(tuple_to_tensor(typename reverse<shape>::type{}, std::make_index_sequence<rank>{}));

        /// 
        /// \brief  Contains the elements of the tensor in a contiguous array. 
        /// 
        /// \details    Refer to compute_index to understand the translation between tensor subscript indices and the linearized storage index
        /// 
        using linearized_container = std::array<element_t_, size>;
        linearized_container data;


        using iterator = typename linearized_container::iterator;
        using const_iterator = typename linearized_container::const_iterator;
        using reverse_iterator = typename linearized_container::reverse_iterator;
        using const_reverse_iterator = typename linearized_container::const_reverse_iterator;

        [[nodiscard]] constexpr iterator begin() noexcept { return data.begin(); }
        [[nodiscard]] constexpr iterator end() noexcept { return data.end(); }
        [[nodiscard]] constexpr const_iterator begin() const noexcept { return data.begin(); }
        [[nodiscard]] constexpr const_iterator end() const noexcept { return data.end(); }
        [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return data.cbegin(); }
        [[nodiscard]] constexpr const_iterator cend() const noexcept { return data.cend(); }

        [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return data.rbegin(); }
        [[nodiscard]] constexpr reverse_iterator rend() noexcept { return data.rend(); }
        [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return data.rbegin(); }
        [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return data.rend(); }
        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return data.crbegin(); }
        [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return data.crend(); }


        /// 
        /// \brief  The default constructor leaves the tensor's elements uninitialized
        /// 
        constexpr tensor() noexcept = default;

        /// 
        /// \brief  
        /// 
        constexpr tensor &operator = (const tensor &other) noexcept = default;

        /// 
        /// \brief  Constructs the tensor from a data array, imposing the tensor's ordering on the elements of the array.
        /// 
        constexpr explicit tensor(const std::array<element_t_, size> &plain_array) noexcept
        : data(plain_array)
        {

        }


        /// 
        /// \brief  Constructs the tensor from a parameter pack of elements
        /// 
        template<typename... elements_>
        constexpr tensor(elements_ &&... elements) noexcept
        requires (sizeof...(elements_) == size && (std::convertible_to<elements_, element_t_> && ...))
        {
            element_t_ array_[] = { static_cast<element_t_>(std::forward<elements_>(elements))... };
            std::copy(std::begin(array_), std::end(array_), data.begin());
        }

        /// 
        /// \brief  For size 1 tensors, a constructor that takes the scalar element directly. 
		///         Note that to create a non-scalar tensor from a single value, use the fill constructor below.
        /// 
        constexpr explicit tensor(const element_t_ &scalar) noexcept
        : data()
        {
            static_assert(size == 1, "Tensor must be a single element to construct from scalar.");
            data[0] = scalar;
        }

        /// 
        /// 
        /// 
        [[nodiscard]] constexpr element_t_ scalar() const noexcept requires ((dimensions == 1) && ...)
        {
            static_assert(size == 1, "Tensor must be a single element to cast to scalar."); // this may not be necessary, since we already have the `requires` clause
            return data[0];
        }

        [[nodiscard]] explicit constexpr operator element_t_() const noexcept requires ((dimensions == 1) && ...)
        {
            return scalar();
        }



        /// 
        /// \brief  Default destructor deallocates the data array
        /// 
        constexpr ~tensor() noexcept = default;

        /// 
        /// \brief  Cast all the elements in the tensor to another type, resulting in a new tensor. Will do narrowing conversions without warning
        /// 
        template<typename target_type_>
        [[nodiscard]] constexpr tensor<target_type_, dimensions...> as() const noexcept
        {
            if constexpr(std::is_same_v< target_type_, element_t_>){
                return *this; // quick copy
            }

            tensor<target_type_, dimensions...> result_;
            std::ranges::transform( data
                                  , result_.data.begin()
                                  , [](const element_t_ &v) { return static_cast<target_type_>(v); }
                                  );
            return result_;
        }

        /// 
        /// \brief  Change the shape of the tensor to a tensor with the same number of elements but different sizes of the dimensions.
        ///         The correspondence between indices in the original tensor and that of the resulting tensor are complex, for more
        ///         principled transformations consider `transpose`, or `slice` or `expand`.
        /// 
        template<std::size_t... new_dimensions>
        [[nodiscard]] constexpr auto reshape() const noexcept
        {
            static_assert((... * new_dimensions) == size, "New shape must have the same total size");

            if constexpr(std::is_same_v<std::index_sequence<new_dimensions...>, std::index_sequence<dimensions...>>){
                return *this; // tensor was unchanged, do a quick copy
            }

            tensor<element_t_, new_dimensions...> result;
            std::copy(data.begin(), data.end(), result.data.begin());
            return result;
        }


        template<typename... ranges_>
        struct ranges
        {
            using type = std::tuple<typename sanitize_range<ranges_, dimensions>::type...>;
        };

        /// 
        /// \brief  Takes one or more ranges to produce a (smaller) tensor with indices restricted to those ranges
        /// 
        template<typename... ranges_>
        [[nodiscard]] constexpr auto slice() const noexcept
        {
            static_assert(sizeof...(ranges_) == rank, "Number of ranges must match the rank of the tensor.");

            using sanitized_ranges_t = typename ranges<ranges_...>::type; // clip ranges to the size of the tensor, when the end point is beyond the max size
            using slice_shape_t      = typename compute_slice_shape<typename sanitize_range<ranges_, dimensions>::type...>::type;
            using result_type        = decltype(tuple_to_tensor(slice_shape_t{}, std::make_index_sequence<std::tuple_size_v<slice_shape_t>>{})); // compute the shape of the result

            auto sanitized_ranges_tuple_ = sanitized_ranges_t{}; // instantiate ranges so that we can operate on them


            result_type result_; // allocate the result

            auto self_assign = [&](auto &result, auto result_indices, auto... original_indices) constexpr -> void
            {
                std::apply([&](auto... result_indices_args)
                {
                    result(result_indices_args...) = (*this)(original_indices...);
                }, result_indices);
            };

            auto fill_slice = [&](auto &self, auto &result, auto result_indices, auto... original_indices) constexpr -> void
            {
                if constexpr(sizeof...(original_indices) == rank){
                    self_assign(result, result_indices, original_indices...);
                }else{
                    constexpr std::size_t dimension_ = sizeof...(original_indices);
                    const auto &range_               = std::get<dimension_>(sanitized_ranges_tuple_);
                    constexpr std::size_t begin      = range_.begin_value;
                    constexpr std::size_t end        = range_.end_value;
                    constexpr std::size_t stride     = range_.stride_value;
                    for(std::size_t i = begin, j = 0; i < end; i += stride, ++j){
                        self(self, result, std::tuple_cat(result_indices, std::tuple(j)), original_indices..., i);
                    }
                }
            };
            fill_slice(fill_slice, result_, std::tuple<>());
            return result_;
        }

        /// 
        /// \brief  Default three-way comparison operator defers to std::array
        /// 
        [[nodiscard]] constexpr auto operator <=> (const tensor &operand) const noexcept = default;


        /// 
        /// \brief  Default comparison operator defers to std::array
        /// 
        [[nodiscard]] constexpr bool operator == (const tensor &operand) const noexcept = default;

        /// 
        /// \brief  
        /// 
        /// \detail This template is permissive when it comes to the type of integer used to the indices, and does a static_cast to to std::size_t.
        ///         This allows users to index without having to type-checking that their indices are std::size_t.
        /// 
        /// 
        template<std::size_t... index_sequence_, typename... indices_t_>
        [[nodiscard]] static constexpr std::size_t compute_index(std::index_sequence<index_sequence_...>, indices_t_... indices) noexcept
        {
            static_assert(sizeof...(indices) == rank, "The number of indices must match the rank of the tensor (even if a dimension has size 1).");

            std::array<std::size_t, rank> indices_array = { static_cast<std::size_t>(indices)... };

            constexpr std::array<std::size_t, rank> dimensions_array = { dimensions... };
            std::size_t index      = 0;
            std::size_t multiplier = 1;

            for(std::size_t i = rank; i > 0; --i){
                if(dimensions_array[i - 1] > 1){
                    index += indices_array[i - 1] * multiplier;
                }
                multiplier *= dimensions_array[i - 1];
            }
            //assert(index < size); // , "Index exceeds tensor raw size.");
            return index;
        }

        /// 
        /// \brief  Static method to onvert indices to linearized index
        /// 
        template<std::size_t... indices>
        [[nodiscard]] static constexpr std::size_t compute_index(const std::array<std::size_t, sizeof...(indices)> &a, std::index_sequence<indices...>) noexcept
        {
            static_assert(sizeof...(indices) == rank, "The number of indices must match the rank of the tensor (even if a dimension has size 1).");
            return compute_index(std::index_sequence<indices...>{}, a[indices]...);
        }

        template<std::size_t N>
        [[nodiscard]] static constexpr std::size_t compute_index(const std::array<std::size_t, N> &a) noexcept
        {
            static_assert(N == rank, "Array size must match tensor rank in compute_index.");
            return[&]<std::size_t... indices_>(std::index_sequence<indices_...>)
            {
                return compute_index(std::index_sequence<indices_...>{}, a[indices_]...);
            }(std::make_index_sequence<rank>{});  // Use 'rank' which refers to the rank of the specific tensor class (i.e.., result_type::rank) this static method belongs to.
        } 

        /// 
        /// \brief  Convert dynamic indices to linearized indices.
        /// 
        template<typename... indices_t_>
        [[nodiscard]] constexpr element_t_ &operator() (indices_t_... indices) noexcept
        {
            return data[compute_index(std::make_index_sequence<rank>(), indices...)];
        }

        /// 
        /// \brief  Element access operator
        /// 
        template<typename... indices_t_>
        [[nodiscard]] constexpr const element_t_ &operator() (indices_t_... indices) const noexcept
        {
            return data[compute_index(std::make_index_sequence<rank>(), indices...)];
        }

        /// 
        /// \brief  Factory method to construct a tensor full elements of the same value
        /// 
        [[nodiscard]] static constexpr tensor<element_t_, dimensions...> full(const element_t_ &value) noexcept
        {
            tensor<element_t_, dimensions...> result_;
            result_.data.fill(value);
            return result_;
        }

        /// 
        /// \brief  Factory method to construct a tensor full of zeroes
        /// 
        [[nodiscard]] static constexpr tensor<element_t_, dimensions...> zeros() noexcept
        {
            return full(element_t_(0));
        }


        /// 
        /// \brief  Factory method to construct a tensor full of ones
        /// 
        [[nodiscard]] static constexpr tensor<element_t_, dimensions...> ones() noexcept
        {
            return full(element_t_(1));
        }


        /// 
        /// \brief  Reference the i-th element of the diagonal.
        /// 
        template<std::size_t i>
        [[nodiscard]] constexpr element_t_ &diagonal() noexcept
        {
            static_assert(i < diagonal_size, "Index must be 0 <= i < diagonal_size.");
            std::size_t index = 0;
            std::size_t multiplier = 1;
            (..., (index += i * multiplier, multiplier *= dimensions));
            return data[index];
        }

        /// 
        /// \brief  Copy the i-th element of the diagonal.
        /// 
        template<std::size_t i>
        [[nodiscard]] constexpr element_t_ diagonal() const noexcept
        {
            static_assert(i < diagonal_size, "Index must be 0 <= i < diagonal_size.");
            std::size_t index = 0;
            std::size_t multiplier = 1;
            (..., (index += i * multiplier, multiplier *= dimensions));
            return data[index];
        }

        /// 
        /// \brief  Set the values on the diagonal, with diagonal indices `i`
        /// 
        template<std::size_t... index_sequence_>
        constexpr auto set_diagonal(const element_t_ &value, std::index_sequence<index_sequence_...>) noexcept
        {
            (..., (diagonal<index_sequence_>() = value));
            return (*this);
        }

        /// 
        /// \brief  Set values on all the elements of the diagonal
        /// 
        constexpr auto set_diagonal(const element_t_ &value) noexcept
        {
            return set_diagonal(value, std::make_index_sequence<diagonal_size>{});
        }

        /// 
        /// \brief  
        /// 
        template<std::size_t... index_sequence_>
        constexpr auto set_diagonal(const tensor<element_t_, diagonal_size> &values, std::index_sequence<index_sequence_...>) noexcept
        {
            (..., (diagonal<index_sequence_>() = values(index_sequence_)));
            return (*this);
        }

        /// 
        /// \brief  Set diagonal, with a tensor with diagonal elements
        /// 
        constexpr auto set_diagonal(const tensor<element_t_, diagonal_size> &values) noexcept
        {
            return set_diagonal(values, std::make_index_sequence<diagonal_size>{});
        }

        /// 
        /// \brief  Extract the diagonal as a tensor
        /// 
        [[nodiscard]] constexpr tensor<element_t_, diagonal_size> get_diagonal() const noexcept
        {
            tensor<element_t_, diagonal_size> result_;

            [&]<std::size_t... indices_>(std::index_sequence<indices_...>)
            {
                ((result_(indices_) = diagonal<indices_>()), ...);
            }(std::make_index_sequence<diagonal_size>{});

            return result_;
        }

        /// 
        /// \brief  The identity tensor has ones on the diagonal, and zeros elsewhere (Kronecker delta tensor)
        /// 
        [[nodiscard]] static constexpr tensor<element_t_, dimensions...> identity() noexcept
        {
            auto result_ = zeros();
            result_.set_diagonal(element_t_(1));
            return result_;
        }

        ///
        /// \brief  Negation 
        ///
        [[nodiscard]] constexpr auto operator -() const noexcept
        {
            tensor<element_t_, dimensions...> result_;
            std::transform( data.begin()
                          , data.end()
                          , result_.data.begin()
                          , [](const element_t_ &element)
                            {
                                return -element;
                            });
            return result_;
        }

        /// 
        /// 
        template<std::convertible_to<element_t_> scalar_t_>
        constexpr auto &operator += (const scalar_t_ &scalar) noexcept
        {
            std::ranges::transform( data
                                  , data.begin()
                                  , [scalar](const element_t_ &element)
                                    {
                                        return element + static_cast<element_t_>(scalar);
                                    });
            return *this;
        }

        template<std::convertible_to<element_t_> scalar_t_>
        [[nodiscard]] constexpr auto operator + (const scalar_t_ &scalar) const noexcept
        {
            tensor<element_t_, dimensions...> result_(*this);
            result_ += scalar;
            return result_;
        }

    
        template<std::convertible_to<element_t_> scalar_t_>
        constexpr auto operator -= (const scalar_t_ &scalar) noexcept
        {
            std::ranges::transform( data
                                  , data.begin()
                                  , [scalar](const element_t_ &element)
                                    {
                                        return element - static_cast<element_t_>(scalar);
                                    });
            return *this;
        }

        template<std::convertible_to<element_t_> scalar_t_>
        [[nodiscard]] constexpr auto operator - (const scalar_t_ &scalar) const noexcept
        {
            tensor<element_t_, dimensions...> result_(*this);
            result_ -= scalar;
            return result_;
        }


        template<std::convertible_to<element_t_> scalar_t_>
        constexpr auto operator *= (const scalar_t_ &scalar) noexcept
        {
            std::ranges::transform( data
                                  , data.begin()
                                  , [scalar](const element_t_ &element)
                                    {
                                        return element * static_cast< element_t_>(scalar);
                                    });
            return *this;
        }

        template<std::convertible_to<element_t_> scalar_t_>
        [[nodiscard]] constexpr auto operator * (const scalar_t_ &scalar) const noexcept
        {
            tensor<element_t_, dimensions...> result_(*this);
            result_ *= scalar;
            return result_;
        }


        template<std::convertible_to<element_t_> scalar_t_>
        constexpr auto operator /= (const scalar_t_ &scalar) noexcept
        {
            std::ranges::transform( data
                                  , data.begin()
                                  , [scalar](const element_t_ &element)
                                    {
                                        return element / static_cast<element_t_>(scalar);
                                    });
            return *this;
        }

        template<std::convertible_to<element_t_> scalar_t_>
        [[nodiscard]] constexpr auto operator / (const scalar_t_ &scalar) const noexcept
        {
            tensor<element_t_, dimensions...> result_(*this);
            result_ /= scalar;
            return result_;
        }


        ///
        /// \brief  += operator for tensors with identical dimensions
        ///
        constexpr auto &operator += (const tensor<element_t_, dimensions...> &other) noexcept
        {
            std::transform( std::begin(data)
                          , std::end(data)
                          , std::begin(other.data)
                          , std::begin(data)
                          , std::plus<element_t_>());
            return *this;
        }

        /// 
        ///
        /// 
        [[nodiscard]] constexpr auto operator + (const tensor<element_t_, dimensions...> &other) const noexcept
        {
            tensor<element_t_, dimensions...> result_(*this);
            result_ += other;
            return result_;
        }

        /// 
        /// \brief  General case for operator +, where the dimensions do not match, but can be broadcasted still
        /// 
        template<std::size_t... other_dimensions>
        constexpr auto operator + (const tensor<element_t_, other_dimensions...> &other) const noexcept
        {
            return broadcast_operation(other, std::plus<element_t_>());
        }

        ///
        /// \brief
        ///
        constexpr auto &operator -= (const tensor<element_t_, dimensions...> &other) noexcept
        {
            std::transform( std::begin(data)
                          , std::end(data)
                          , std::begin(other.data)
                          , std::begin(data)
                          , std::minus<element_t_>()
                          );
            return *this;
        }

        /// 
        /// \brief
        /// 
        [[nodiscard]] constexpr auto operator - (const tensor<element_t_, dimensions...> &other) const noexcept
        {
            tensor<element_t_, dimensions...> result_(*this);
            result_ -= other;
            return result_;
        }

        template<std::size_t... other_dimensions>
        constexpr auto operator - (const tensor<element_t_, other_dimensions...> &other) const noexcept
        {
            return broadcast_operation(other, std::minus<element_t_>());
        }

        ///
        /// \brief
        ///
        constexpr auto &operator *= (const tensor<element_t_, dimensions...> &other) noexcept
        {
            std::transform(std::begin(data)
                          , std::end(data)
                          , std::begin(other.data)
                          , std::begin(data)
                          , std::multiplies<element_t_>()
                          );
            return *this;
        }

        /// 
        /// \brief 
        /// 
        [[nodiscard]] constexpr auto operator * (const tensor<element_t_, dimensions...> &other) const noexcept
        {
            tensor<element_t_, dimensions...> result_(*this);
            result_ *= other;
            return result_;
        }

        template<std::size_t... other_dimensions>
        constexpr auto operator * (const tensor<element_t_, other_dimensions...> &other) const noexcept
        {
            return broadcast_operation(other, std::multiplies<element_t_>());
        }

        ///
        /// \brief  
        ///
        constexpr auto &operator /= (const tensor<element_t_, dimensions...> &other) noexcept
        {
            std::transform(std::begin(data)
                          , std::end(data)
                          , std::begin(other.data)
                          , std::begin(data)
                          , std::divides<element_t_>()
                          );
            return *this;
        }

        /// 
        /// \brief  
        /// 
        [[nodiscard]] constexpr auto operator / (const tensor<element_t_, dimensions...> &other) const noexcept
        {
            tensor<element_t_, dimensions...> result_(*this);
            result_ /= other;
            return result_;
        }

        /// 
        /// \brief  
        /// 
        template<std::size_t... other_dimensions>
        constexpr auto operator / (const tensor<element_t_, other_dimensions...> &other) const noexcept
        {
            return broadcast_operation(other, std::divides<element_t_>());
        }

        /// 
        /// 
        /// 
        constexpr tensor<element_t_, dimensions...> &apply_in_place(std::function<element_t_(const element_t_ &)> function)
        {
            std::ranges::transform(data, data.begin(), function);
            return *this;
        }

        /// 
        /// \brief Applies a function to each element independently
        /// 
        template<typename function_t_>
        requires std::invocable<function_t_, const element_t_ &>
        constexpr auto apply(function_t_ function) const
        noexcept(noexcept(std::declval<function_t_>()(std::declval<const element_t_ &>())))
        {
            using result_t_ = std::invoke_result_t<function_t_, const element_t_ &>;
            tensor<result_t_, dimensions...> result_;
            std::ranges::transform(data.begin(), data.end(), result_.data.begin(), function);
            return result_;
        }

        /// 
        /// \brief  
        /// 
        [[nodiscard]] constexpr tensor<element_t_, dimensions...> &replace_in_place(const element_t_ &value, const element_t_ &replacement) noexcept
        {
            std::ranges::replace_if(data, [&](const element_t_ &x) { return value == x; }, replacement);
            return *this;
        }

        /// 
        /// \brief  
        /// 
        [[nodiscard]] constexpr element_t_ sum() const noexcept
        {
            return std::accumulate(data.begin(), data.end(), element_t_{ 0 }, std::plus<element_t_>());
        }

        /// 
        /// 
        /// 
        [[nodiscard]] constexpr element_t_ product() const noexcept
        {
            return std::accumulate(data.begin(), data.end(), element_t_{ 1 }, std::multiplies<element_t_>());
        }

        /// 
        /// \brief  Fastest method to get the mean, but the result may be unstable if the sum is too large to represent.
        /// 
        [[nodiscard]] constexpr element_t_ mean() const noexcept
        {
            return sum() / size;
        }

        /// 
        /// \brief  Welford's algorithm for numerically stable mean.
        /// 
        constexpr element_t_ stable_mean() noexcept
        {            
            if constexpr(0 == size){
                return element_t_{ 0 };
            }
            element_t_ result_{ 0 };
            for(std::size_t i = 0; i < size; ++i){
                result_ += ((data[i]) - result_) / static_cast<element_t_> (i + 1);
            }
            return result_;
        }

        [[nodiscard]] constexpr element_t_ max() const noexcept // inferred noexcept, because tensor can't be empty
        {
            return *std::max_element(data.begin(), data.end());
        }

        [[nodiscard]] constexpr element_t_ min() const noexcept // inferred noexcept, because tensor can't be empty
        {
            return *std::min_element(data.begin(), data.end());
        }

        /// 
        /// \brief Frobenius norm
        /// 
        [[nodiscard]] constexpr element_t_ frobenius_norm() const noexcept
        {
            auto squared_ = (*this) * (*this);
            element_t_ sum_ = squared_.sum(); // ensure sum is a scalar
            return std::sqrt(sum_);
        }


        template<std::size_t rank_>
        [[nodiscard]] static constexpr std::array<std::size_t, rank_> compute_broadcasted_indices(std::size_t index, const std::array<std::size_t, rank_> &dim_sizes)
        {
            std::array<std::size_t, rank_> indices_ = {};
            for(std::size_t i = 0; i < rank_; ++i){
                indices_[rank_ - 1 - i] = index % dim_sizes[rank_ - 1 - i];
                index /= dim_sizes[rank_ - 1 - i];
            }
            return indices_;
        }

        template<std::size_t rank_>
        [[nodiscard]] static constexpr std::size_t compute_broadcasting_index(const std::array<std::size_t, rank_> &indices, const std::array<std::size_t, rank_> &dim_sizes) noexcept
        {
            std::size_t flat_index_ = 0;
            std::size_t stride_ = 1;
            for(std::size_t i = rank_; i-- > 0;){
                flat_index_ += indices[i] * stride_;
                stride_ *= dim_sizes[i];
            }
            return flat_index_;
        }

        /// 
        /// \brief  
        /// 
        template<typename operator_t_, std::size_t... other_dimensions>
        [[nodiscard]] constexpr auto broadcast_operation(const tensor<element_t_, other_dimensions...> &other, operator_t_ binary_operator) const
        {
            static_assert(sizeof...(dimensions) == sizeof...(other_dimensions), "Tensors must have the same number of dimensions for broadcasting.");
            constexpr bool is_broadcastable = ((dimensions == other_dimensions || other_dimensions == 1 || dimensions == 1) && ...);
            static_assert(is_broadcastable, "Dimensions must either match or be 1 for broadcasting.");
            constexpr std::size_t result_size = (... * (std::max(dimensions, other_dimensions)));

            constexpr std::array<std::size_t, sizeof...(dimensions)> lhs_dim_sizes       = { dimensions... };
            constexpr std::array<std::size_t, sizeof...(other_dimensions)> rhs_dim_sizes = { other_dimensions... };
            constexpr std::array<std::size_t, sizeof...(dimensions)> result_dim_sizes    = { std::max(dimensions, other_dimensions)... };

            std::array<element_t_, result_size> result_data = {};
            for(std::size_t i = 0; i < result_size; ++i){
                auto result_indices = compute_broadcasted_indices(i, result_dim_sizes);

                std::array<std::size_t, sizeof...(dimensions)> lhs_indices = {};
                std::array<std::size_t, sizeof...(other_dimensions)> rhs_indices = {};

                for(std::size_t j = 0; j < sizeof...(dimensions); ++j){
                    lhs_indices[j] = (lhs_dim_sizes[j] == 1) ? 0 : result_indices[j];
                    rhs_indices[j] = (rhs_dim_sizes[j] == 1) ? 0 : result_indices[j];
                }

                auto lhs_index = compute_broadcasting_index(lhs_indices, lhs_dim_sizes);
                auto rhs_index = compute_broadcasting_index(rhs_indices, rhs_dim_sizes);

                result_data[i] = binary_operator(data[lhs_index], other.data[rhs_index]);
            }

            return tensor<element_t_, (std::max(dimensions, other_dimensions))...>{result_data};
        }

        /// 
        /// \brief  A reduction/fold along a given dimension
        /// 
        template<std::size_t reduction_dimension, typename operator_t_>
        [[nodiscard]] constexpr auto reduce_operation(operator_t_ operation, element_t_ initial) const
        {
            constexpr std::array<std::size_t, rank> dimensions_ = { dimensions... };
            constexpr auto new_dims = [dimensions_]() constexpr -> std::array<std::size_t, rank - 1> // Build a compile‐time array of dimensions excluding the reduced one.
            {
                std::array<std::size_t, rank - 1> reshaped_{};
                for(std::size_t i = 0, j = 0; i < rank; ++i){
                    if(i != reduction_dimension){
                        reshaped_[j++] = dimensions_[i];
                    }
                }
                return reshaped_;
            }();

            // Compute the product of all dimensions except at index 'reduction_dimension'.
            constexpr std::size_t result_size = [dimensions_]<std::size_t... indices_>(std::index_sequence<indices_...>) constexpr
            {
                return ((indices_ == reduction_dimension ? std::size_t{ 1 } : dimensions_[indices_]) * ...);
            }(std::make_index_sequence<rank>{});

            tensor<element_t_, result_size> result_{};
            for(std::size_t i = 0; i < result_size; ++i){
                result_.data[i] = initial;
            }
            
            for(std::size_t flat_index = 0; flat_index < size; ++flat_index){
                auto multi_idx = compute_dimensional_index(flat_index);
                std::array<std::size_t, rank - 1> result_multi_index_{};
                for(std::size_t i = 0, j = 0; i < rank; ++i){
                    if(i != reduction_dimension){
                        result_multi_index_[j++] = multi_idx[i];
                    }
                }
                
                std::size_t result_flat_index_ = 0; // Compute the flat index in the reduced tensor (row–major).
                std::size_t multiplier_ = 1;
                for(std::size_t i = new_dims.size(); i > 0; --i){ //for(long int i = static_cast<long int>(new_dims.size()) - 1; i >= 0; --i){
                    result_flat_index_ += result_multi_index_[i - 1] * multiplier_; //result_flat_index_ += result_multi_index_[i] * multiplier; //multiplier_ *= new_dims[i];
                    multiplier_ *= new_dims[i - 1];
                }
                result_.data[result_flat_index_] = operation(result_.data[result_flat_index_], data[flat_index]);
            }
            return result_;
        }

        /// 
        /// \brief  The sum along dimension_
        /// 
        template<std::size_t dimension_>
        [[nodiscard]] constexpr auto sum() const noexcept
        {
            return reduce_operation<dimension_>(std::plus<element_t_>(), element_t_(0));
        }

        /// 
        /// \brief   Mean along dimension_
        /// 
        template<std::size_t dimension_>
        [[nodiscard]] constexpr auto mean() const noexcept
        {
            auto dimension_sum_ = reduce_operation<dimension_>(std::plus<element_t_>(), element_t_(0));
            constexpr std::size_t array_[] = { dimensions... };
            return dimension_sum_ / element_t_(array_[dimension_]);
        }


        /// 
        /// \brief  The maximum value along dimension_
        /// 
        template<std::size_t dimension_>
        [[nodiscard]] constexpr auto max() const noexcept
        {
            return reduce_operation<dimension_>([](const element_t_ &a, const element_t_ &b) { return std::max(a, b); }, std::numeric_limits<element_t_>::lowest());
        }

        /// 
        /// \brief  The minimum value along dimension_
        /// 
        template<std::size_t dimension_>
        [[nodiscard]] constexpr auto min() const noexcept
        {
            return reduce_operation<dimension_>([](const element_t_ &a, const element_t_ &b) { return std::min(a, b); }, std::numeric_limits<element_t_>::max());
        }

        /// 
        /// \brief  Given a flat (or linear) index, compute the dimensional index, which is an array
        /// 
        [[nodiscard]] constexpr static std::array<std::size_t, rank> compute_dimensional_index(std::size_t flat_index) noexcept
        {
            std::array<std::size_t, rank> multi_index{};
            std::size_t dimensions_array[] = { dimensions... };
            std::size_t current_index = flat_index;

            for(std::size_t i = rank; i > 0; --i){
                multi_index[i - 1] = current_index % dimensions_array[i - 1];
                current_index /= dimensions_array[i - 1];
            }
            return multi_index;
        }

        /// 
        /// \brief  Transposes the tensor. The transpose gives a tensor of the same shape, with all dimensions in reversed order
        /// 
        [[nodiscard]] constexpr auto transpose() const noexcept
        {
            using result_t = typename tensor<element_t_, dimensions...>::transposed_type;
            auto result_ = result_t();
            for(std::size_t i = 0; i < size; ++i){
                auto multi_index = compute_dimensional_index(i);
                std::reverse(multi_index.begin(), multi_index.end());
                std::size_t j = result_t::compute_index(multi_index, std::make_index_sequence<rank>{});
                result_.data[j] = data[i];
            }
            return result_;
        }

    private:
        // Helper to get dimension value by index from a non-type parameter pack
        template<std::size_t index_, std::size_t... bounds_>
        struct get_dimension
        {
            static_assert(index_ < sizeof...(bounds_), "Index out of bounds");
            
            static constexpr std::array<std::size_t, sizeof...(bounds_)> bounds_array_ = { bounds_... };

            static constexpr std::size_t value = bounds_array_[index_];
        };

        // Filter indices and build resulting dimension sequence of *values*
        // Parameters: remove_ (index to remove), sequence_input_ (0..N-1), sequence_output_ (result accumulator), original_dimensions_... (the dimension values)
        template <std::size_t remove_, typename sequence_input_, typename sequence_output_, std::size_t... original_dimensions_> //  Last is non-type pack
        struct filter_dimension; // Forward declaration only

        // Recursive step
        template <std::size_t remove_, std::size_t first_, std::size_t... rest_, std::size_t... keep_, std::size_t... original_dimensions_>
        struct filter_dimension<remove_, std::index_sequence<first_, rest_...>, std::index_sequence<keep_...>, original_dimensions_...>
        {
            // Get the actual dimension value corresponding to first_ from the original_dimensions_
            static constexpr std::size_t current_dimension_value = get_dimension<first_, original_dimensions_...>::value;

            // Decide whether to keep the current dimension value based on its original *index*
            using keep_dimensions = std::conditional_t< (first_ == remove_)
                                                      , std::index_sequence<keep_...>                   // Skip: Index matches remove_
                                                      , std::index_sequence<keep_..., current_dimension_value> // Keep: Index doesn't match
                                                      >;

            // Recurse with remaining indices and updated kept dimensions
            using type = typename filter_dimension<remove_, std::index_sequence<rest_...>, keep_dimensions, original_dimensions_...>::type;
        };

        // Base case: Input index sequence is empty
        template <std::size_t remove_, std::size_t... keep_, std::size_t... original_dimensions_>
        struct filter_dimension<remove_, std::index_sequence<>, std::index_sequence<keep_...>, original_dimensions_...>
        {
            using type = std::index_sequence<keep_...>; // Final sequence of kept dimension values
        };


        // Build the final result tensor type from two filtered dimension sequences (of values)
        template <typename left_, typename right_>
        struct contraction_result; // forward declaration

        template <std::size_t... left_, std::size_t... right_>
        struct contraction_result<std::index_sequence<left_...>, std::index_sequence<right_...>>
        {
            static constexpr std::size_t result_rank_val = sizeof...(left_) + sizeof...(right_);

            // Define the result type using the concatenated dimension values
            using type = std::conditional_t< result_rank_val == 0
                                           , tensor<element_t_, 1> // Scalar case results in tensor<T, 1>
                                           , tensor<element_t_, left_..., right_...> // N-D case
                                           >;
        };


    public:
        template< std::size_t dimension_left
                , std::size_t dimension_right
                , typename element_other_t_
                , std::size_t... other_dimensions
                , typename binary_function_t_ = std::multiplies<> // Use fully specified types for defaults for clarity if needed, though <> works C++17+
                , typename reduce_function_t_ = std::plus<>
                >
        [[nodiscard]] constexpr auto contraction( const tensor<element_other_t_, other_dimensions...> &other
                                                , binary_function_t_ binary_op = binary_function_t_{}
                                                , reduce_function_t_ reduce_op = reduce_function_t_{}
                                                , element_t_ initial_value = element_t_{ 0 }
                                                ) const
            // Add noexcept based on ops only if guaranteed
            // noexcept(noexcept(binary_op(std::declval<element_t_>(), std::declval<element_other_t_>())) &&
            //          noexcept(reduce_op(std::declval<element_t_>(), std::declval<decltype(binary_op(element_t_{}, element_other_t_{}))>() )))
        {
            constexpr std::size_t other_rank = sizeof...(other_dimensions);
            static_assert(dimension_left < rank, "Left contraction dimension index out of bounds.");
            static_assert(dimension_right < other_rank, "Right contraction dimension index out of bounds.");

            constexpr auto left_contract_dim_size  = get_dimension<dimension_left, dimensions...>::value;
            constexpr auto right_contract_dim_size = get_dimension<dimension_right, other_dimensions...>::value;
            static_assert(left_contract_dim_size == right_contract_dim_size, "Contracted dimensions must have the same size.");

            // --- Determine Result Type using In-Place Helpers ---
            using left_filtered_dims_seq = typename filter_dimension<
                dimension_left, std::make_index_sequence<rank>, std::index_sequence<>, dimensions...
            >::type;
            using right_filtered_dims_seq = typename filter_dimension<
                dimension_right, std::make_index_sequence<other_rank>, std::index_sequence<>, other_dimensions...
            >::type;
            using result_type = typename contraction_result<left_filtered_dims_seq, right_filtered_dims_seq>::type;

            // Calculate logical rank (rank before scalar handling) for indexing math
            constexpr std::size_t logical_result_rank = rank + other_rank - 2;


            result_type result{};
            if constexpr(result_type::size > 0){
                std::fill(result.data.begin(), result.data.end(), initial_value);
            }

            // Iterate through all elements of the *left* tensor
            for(std::size_t i = 0; i < this->size; ++i){
                auto left_multi_indices = this->compute_dimensional_index(i);
                std::size_t k_left = left_multi_indices[dimension_left];

                // Iterate through all elements of the *right* tensor
                for(std::size_t j = 0; j < other.size; ++j){
                    auto right_multi_indices = other.compute_dimensional_index(j);
                    std::size_t k_right = right_multi_indices[dimension_right];

                    // Only proceed if the indices along the contracted dimensions match
                    if(k_left == k_right){
                        // Calculate the multi-dimensional index in the result tensor
                        std::array<std::size_t, logical_result_rank> result_multi_indices{}; // Use logical rank
                        std::size_t current_result_idx = 0;

                        // Add non-contracted indices from the left tensor
                        for(std::size_t l_idx = 0; l_idx < rank; ++l_idx){
                            if(l_idx != dimension_left){
                                if constexpr(logical_result_rank > 0){
                                    result_multi_indices[current_result_idx++] = left_multi_indices[l_idx];
                                }
                            }
                        }
                        // Add non-contracted indices from the right tensor
                        for(std::size_t r_idx = 0; r_idx < other_rank; ++r_idx){
                            if(r_idx != dimension_right){
                                if constexpr(logical_result_rank > 0){
                                    result_multi_indices[current_result_idx++] = right_multi_indices[r_idx];
                                }
                            }
                        }

                        // Compute the flat index in the result tensor
                        std::size_t result_flat_index = 0; // Default index for scalar result (tensor<T,1>)
                        if constexpr(logical_result_rank > 0){
                            // Call compute_index on the result_type using the array overload
                            result_flat_index = result_type::compute_index(result_multi_indices);
                        }
                        // else: scalar case, result_flat_index remains 0

                        // Apply the binary operation and then the reduction operation
                        // Using auto for intermediate result for flexibility with op return types
                        auto binary_result = binary_op(this->data[i], other.data[j]);
                        result.data[result_flat_index] = reduce_op(
                            result.data[result_flat_index],       // Current accumulated value (element_t_)
                            binary_result                         // Result of binary op
                        );
                    }
                }
            }
            return result;
        }


        /// 
        /// \brief  Sort tensor elements based on indices along specified dimensions
        ///         C++20 constexpr 
        /// 
        template<std::size_t... by>
        [[nodiscard]] constexpr tensor<element_t_, dimensions...> sort() const
        {
            tensor<element_t_, dimensions...> result_ = *this;
            std::array<std::size_t, size> indices_;
            std::iota(indices_.begin(), indices_.end(), 0);

            // Sort indices based on dimensional indices
            std::sort(indices_.begin()
                     , indices_.end()
                     , [&](std::size_t a, std::size_t b)
                       {
                            // Get multi-dimensional indices
                            auto a_indices = result_.compute_dimensional_index(a);
                            auto b_indices = result_.compute_dimensional_index(b);

                            if constexpr(sizeof...(by) == 0){ 
                                for(int d = rank - 1; d >= 0; --d){
                                    if(a_indices[d] < b_indices[d]){ // Default: compare dimensions in reverse order (most significant first)
                                        return true; 
                                    }else if(a_indices[d] > b_indices[d]){ 
                                        return false;
                                    }
                                }
                                return false;
                            }//else{
                            return compare_indices<by...>(a_indices, b_indices); // Compare using specified dimensions in given order
                            //}
                        });

            for(std::size_t i = 0; i < size; ++i){
                result_.data[i] = result_.data[indices_[i]];
            }
            return result_;
        }

        /// 
        /// \brief  Stable-sort tensor elements based on indices along specified dimensions
        ///         C++26 constexpr 
        /// 
        template<std::size_t... by>
        [[nodiscard]] tensor<element_t_, dimensions...> stable_sort() const
        {
            tensor<element_t_, dimensions...> result_ = *this;
            std::array<std::size_t, size> indices_;
            std::iota(indices_.begin(), indices_.end(), 0);

            // Sort indices based on dimensional indices
            std::stable_sort(indices_.begin()
                , indices_.end()
                , [&](std::size_t a, std::size_t b)
                {
                    // Get multi-dimensional indices
                    auto a_indices = result_.compute_dimensional_index(a);
                    auto b_indices = result_.compute_dimensional_index(b);

                    if constexpr(sizeof...(by) == 0){
                        for(int d = rank - 1; d >= 0; --d){
                            if(a_indices[d] < b_indices[d]){ // Default: compare dimensions in reverse order (most significant first)
                                return true;
                            } else if(a_indices[d] > b_indices[d]){
                                return false;
                            }
                        }
                        return false;
                    }//else{
                    return compare_indices<by...>(a_indices, b_indices); // Compare using specified dimensions in given order
                    //}
                });

            for(std::size_t i = 0; i < size; ++i){
                result_.data[i] = result_.data[indices_[i]];
            }
            return result_;
        }

        template<std::size_t first, std::size_t... rest>
        [[nodiscard]] static constexpr bool compare_indices(const std::array<std::size_t, rank> &a, const std::array<std::size_t, rank> &b)
        {
            // Compare at the specified dimension
            if(a[first] < b[first]){
                return true;
            }else if(a[first] > b[first]){
                return false;
            }
            if constexpr(sizeof...(rest) > 0){ // If equal and we have more dimensions to check, continue
                return compare_indices<rest...>(a, b);
            }

            return false; // Equal at all specified dimensions
        }

        template<std::size_t position>
        [[nodiscard]] constexpr auto expand() noexcept
        {
            static_assert(position <= sizeof...(dimensions), "Position out of bounds for tensor expansion");

            // Using a lambda to handle the type computation inline
            auto make_expanded_tensor = []<std::size_t... indices>(std::index_sequence<indices...>)
            {
                constexpr auto expanded_dims = []()
                    {
                        std::array<std::size_t, sizeof...(dimensions) + 1> dims{};
                        std::array<std::size_t, sizeof...(dimensions)> original_dims{ dimensions... };

                        // Copy dimensions with insertion of 1 at position pos
                        for(std::size_t i = 0; i < dims.size(); ++i){
                            if(i < position){
                                dims[i] = original_dims[i];
                            } else if(i == position){
                                dims[i] = 1;
                            } else{
                                dims[i] = original_dims[i - 1];
                            }
                        }
                        return dims;
                    }();

                // Extract the dimension values at compile time
                return tensor<element_t_, expanded_dims[indices]...>{};
            };

            // Create the expanded tensor type
            using expanded_tensor_type = decltype(make_expanded_tensor(
                std::make_index_sequence<sizeof...(dimensions) + 1>{}));

            // Create and return the result
            expanded_tensor_type result;
            result.data = data; // Direct data copy since total size is the same
            return result;
        }
    };

    static_assert(std::is_trivially_copyable_v<tensor<double, 1, 1>>);
    static_assert(std::is_standard_layout_v   <tensor<double, 1, 1>>);

    // Non-member operators for scalar-tensor operations
    template<typename element_t_, std::size_t... dimensions>
    constexpr auto operator + (const element_t_ &scalar, const tensor<element_t_, dimensions...> &t)
    {
        return t + scalar;  // Reuse existing operator
    }

    template<typename element_t_, std::size_t... dimensions>
    constexpr auto operator-(const element_t_ &scalar, const tensor<element_t_, dimensions...> &t)
    {
        // Scalar - tensor is not commutative
        tensor<element_t_, dimensions...> result_;
        std::transform(t.data.begin(), t.data.end(), result_.data.begin(),
            [scalar](const element_t_ &element)
            {
                return scalar - element;
            });
        return result_;
    }

    template<typename element_t_, std::size_t... dimensions>
    constexpr auto operator * (const element_t_ &scalar, const tensor<element_t_, dimensions...> &t)
    {
        return t * scalar;  // Reuse existing operator
    }

    template<typename element_t_, std::size_t... dimensions>
    constexpr auto operator / (const element_t_ &scalar, const tensor<element_t_, dimensions...> &t)
    {
        // Scalar / tensor is not commutative
        tensor<element_t_, dimensions...> result_;
        std::transform( t.data.begin()
                      , t.data.end()
                      , result_.data.begin()
                      , [scalar](const element_t_ &element)
                        {
                            return scalar / element;
                        });
        return result_;
    }


    ///
    /// \brief Type trait to check if a type is a tensor
    /// 
    template<typename type_>
    struct is_tensor: std::false_type {};

    template<typename element_t_, std::size_t... dimensions>
    struct is_tensor<tensor<element_t_, dimensions...>>: std::true_type {};

    template<typename type_>
    inline constexpr bool is_tensor_v = is_tensor<type_>::value;


}//namespace esl::mathematics

#include <esl/mathematics/scalar.hpp>
#include <esl/mathematics/vector.hpp>
#include <esl/mathematics/matrix.hpp>

#endif//ESL_MATHEMATICS_TENSOR