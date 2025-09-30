#ifndef ESL_MATHEMATICS_DIFFERENTIABLE_TENSOR
#define ESL_MATHEMATICS_DIFFERENTIABLE_TENSOR

#include <esl/mathematics/tensor.hpp>
#include <esl/mathematics/floating_point.hpp>
#include <esl/mathematics/stochastic.hpp>

namespace esl::mathematics{
    /// 
    /// \brief Tensor that supports automatic differentiation by tracking derivatives
    ///        with respect to parameters.
    /// 
    /// \detail tensor_type must be a type `esl::mathematics::tensor<typename element_type, std::size_t ... dimensions>`. 
    /// 
    template<typename tensor_type_, std::size_t parameters_>
    requires requires { typename tensor_type_::element_type; } // Ensure tensor_type has an element_type
    class differentiable_tensor 
    {
    public:
        /// 
        /// \brief The type of the tensor contained in this object
        /// 
        using tensor_type = tensor_type_;

        /// 
        /// \brief The numerical model for the elements of the contained tensor
        /// 
        using element_type = typename tensor_type::element_type;

        /// 
        /// \brief The rank of the underlying tensor
        ///
        static constexpr std::size_t rank = tensor_type::rank;

        /// 
        /// \brief The size of the underlying tensor
        ///
        static constexpr std::size_t size = tensor_type::size;

        /// 
        /// \brief The number of parameters tracked for differentiation
        ///
        static constexpr std::size_t parameters = parameters_;

        /// 
        /// \brief The contained tensor value
        /// 
        tensor_type value;

        /// 
        /// \brief The derivatives of the contained tensor's values to the parameters
        /// 
        std::array<tensor_type, parameters_> derivatives;

        /// 
        /// \brief Default constructor initializes tensors with default values
        /// 
        constexpr differentiable_tensor() noexcept = default;

        /// 
        /// \brief Assignment operator
        /// 
        constexpr differentiable_tensor &operator = (const differentiable_tensor &other) noexcept = default;

        /// 
        /// \brief Constructor taking a value and derivatives
        /// \note Using a single constructor to avoid ambiguity
        ///
        constexpr differentiable_tensor(const tensor_type &value, const std::array<tensor_type, parameters_> &derivatives) noexcept
        : value(value)
        , derivatives(derivatives)
        {
        }

        /// 
        /// \brief Copy constructor
        /// 
        constexpr differentiable_tensor(const differentiable_tensor &) noexcept(std::is_nothrow_copy_constructible_v<tensor_type>) = default;

        /// 
        /// \brief Move constructor
        /// 
        constexpr differentiable_tensor(differentiable_tensor &&) noexcept(std::is_nothrow_move_constructible_v<tensor_type>) = default;


        ///
        /// \brief Assignment from a raw tensor creates a constant differentiable tensor
        ///
        constexpr differentiable_tensor &operator=(const tensor_type &value) noexcept
        {
            *this = constant(value);
            return *this;
        }

        //
        // Iterators (forward to underlying tensor)
        //
        [[nodiscard]] constexpr auto begin() noexcept { return value.begin(); }
        [[nodiscard]] constexpr auto end() noexcept { return value.end(); }
        [[nodiscard]] constexpr auto begin() const noexcept { return value.begin(); }
        [[nodiscard]] constexpr auto end() const noexcept { return value.end(); }
        [[nodiscard]] constexpr auto cbegin() const noexcept { return value.cbegin(); }
        [[nodiscard]] constexpr auto cend() const noexcept { return value.cend(); }

        [[nodiscard]] constexpr auto rbegin() noexcept { return value.rbegin(); }
        [[nodiscard]] constexpr auto rend() noexcept { return value.rend(); }
        [[nodiscard]] constexpr auto rbegin() const noexcept { return value.rbegin(); }
        [[nodiscard]] constexpr auto rend() const noexcept { return value.rend(); }
        [[nodiscard]] constexpr auto crbegin() const noexcept { return value.crbegin(); }
        [[nodiscard]] constexpr auto crend() const noexcept { return value.crend(); }


        /// 
        /// \brief Get the primal scalar value (for 1-element tensors)
        /// 
        [[nodiscard]] constexpr element_type scalar() const noexcept requires (size == 1)
        {
            return (element_type)value;
        }

        /// 
        /// \brief Get the primal scalar value (for 1-element tensors)
        /// 
        [[nodiscard]] explicit constexpr operator element_type() const noexcept requires (size == 1)
        {
            return scalar();
        }

        /// 
        /// \brief Cast all elements to another type, resulting in a new differentiable tensor
        /// 
        template<typename target_type_>
        [[nodiscard]] constexpr auto as() const noexcept
        {
            using target_tensor_type = decltype(value.template as<target_type_>());
            differentiable_tensor<target_tensor_type, parameters_> result_;

            result_.value = value.template as<target_type_>();
            for(std::size_t i = 0; i < parameters_; ++i){
                result_.derivatives[i] = derivatives[i].template as<target_type_>();
            }

            return result_;
        }


        /// 
        /// \brief Reshape the tensor and its derivatives
        /// 
        template<std::size_t... new_dimensions>
        [[nodiscard]] constexpr auto reshape() const noexcept
        {
            using reshaped_tensor_type = decltype(value.template reshape<new_dimensions...>());
            differentiable_tensor<reshaped_tensor_type, parameters_> result_;

            result_.value = value.template reshape<new_dimensions...>();
            for(std::size_t i = 0; i < parameters_; ++i){
                result_.derivatives[i] = derivatives[i].template reshape<new_dimensions...>();
            }

            return result_;
        }

        /// 
        /// \brief Element access operator (forwards to underlying tensor)
        /// 
        template<typename... indices_t_>
        [[nodiscard]] constexpr const element_type &operator() (indices_t_... indices) const noexcept
        {
            return value(indices...);
        }

        /// 
        /// \brief Get derivative with respect to a parameter at specified indices
        /// 
        template<typename... indices_t_>
        [[nodiscard]] constexpr auto derivative(std::size_t param_index, indices_t_... indices) const noexcept
        {
            return derivatives[param_index](indices...);
        }

        /// 
        /// \brief Get the diagonal of the tensor
        /// 
        [[nodiscard]] constexpr auto get_diagonal() const noexcept
        {
            auto diag_value = value.get_diagonal();
            using diag_tensor_type = decltype(diag_value);

            differentiable_tensor<diag_tensor_type, parameters_> result_;
            result_.value = diag_value;
            for(std::size_t i = 0; i < parameters_; ++i){
                result_.derivatives[i] = derivatives[i].get_diagonal();
            }

            return result_;
        }

        /// 
        /// \brief Create a constant tensor with zero sensitivity to all parameters
        ///
        [[nodiscard]] constexpr static differentiable_tensor constant(const tensor_type &value) noexcept
        {
            differentiable_tensor result_;
            result_.value = value;
            result_.derivatives.fill(tensor_type::zeros());
            return result_;
        }

        ///
        /// \brief Create a tensor that represents a parameter with sensitivity to one parameter
        /// 
        [[nodiscard]] constexpr static differentiable_tensor parameter(const tensor_type &value, std::size_t param_index) noexcept
        {
            differentiable_tensor result_ = constant(value);
            result_.derivatives[param_index] = tensor_type::ones();
            return result_;
        }

        /// 
        /// \brief Create a differentiable tensor filled with zeros
        /// 
        [[nodiscard]] static constexpr differentiable_tensor zeros() noexcept
        {
            return constant(tensor_type::zeros());
        }

        /// 
        /// \brief Create a differentiable tensor filled with ones
        /// 
        [[nodiscard]] static constexpr differentiable_tensor ones() noexcept
        {
            return constant(tensor_type::ones());
        }

        /// 
        /// \brief Create a differentiable tensor filled with a value
        /// 
        [[nodiscard]] static constexpr differentiable_tensor full(const element_type &value) noexcept
        {
            return constant(tensor_type::full(value));
        }

        /// 
        /// \brief Create an identity differentiable tensor (Kronecker delta tensor)
        /// 
        [[nodiscard]] static constexpr differentiable_tensor identity() noexcept
        requires (tensor_type::rank >= 2)
        {
            return constant(tensor_type::identity());
        }

        /// 
        /// \brief Negation/reflection
        /// 
        [[nodiscard]] constexpr differentiable_tensor operator -() const noexcept
        {
            differentiable_tensor result_;
            result_.value = -value;
            for(std::size_t i = 0; i < parameters_; ++i){
                result_.derivatives[i] = -derivatives[i];
            }
            return result_;
        }

        ///
        /// \brief Addition assignment operator
        ///
        constexpr differentiable_tensor &operator += (const differentiable_tensor &other) noexcept
        {
            value += other.value;
            for(std::size_t i = 0; i < parameters_; ++i){
                derivatives[i] += other.derivatives[i];
            }
            return *this;
        }

        ///
        /// \brief Addition operator
        /// 
        [[nodiscard]] constexpr differentiable_tensor operator + (const differentiable_tensor &other) const noexcept
        {
            differentiable_tensor result(*this); // Copy the left operand
            result += other;                     // Use += operator
            return result;
        }

        ///
        /// \brief Subtraction assignment operator
        ///
        constexpr differentiable_tensor &operator -= (const differentiable_tensor &other) noexcept
        {
            value -= other.value;
            for(std::size_t i = 0; i < parameters_; ++i){
                derivatives[i] -= other.derivatives[i];
            }
            return *this;
        }

        /// 
        /// \brief Subtraction operator
        /// 
        [[nodiscard]] constexpr differentiable_tensor operator - (const differentiable_tensor &other) const noexcept
        {
            differentiable_tensor result(*this); // Copy the left operand
            result -= other;                     // Use -= operator
            return result;
        }

        ///
        /// \brief Multiplication assignment operator
        ///
        constexpr differentiable_tensor &operator *= (const differentiable_tensor &other) noexcept
        {
            auto result_value = value * other.value;
            std::array<tensor_type, parameters_> result_derivatives;

            for(std::size_t i = 0; i < parameters_; ++i){
                result_derivatives[i] = derivatives[i] * other.value + value * other.derivatives[i]; // Product rule for derivatives: d(uv)/dx = du/dx * v + u * dv/dx
            }

            value = result_value;
            derivatives = result_derivatives;
            return *this;
        }

       
        /// 
        /// \brief Multiplication operator
        /// 
        [[nodiscard]] constexpr differentiable_tensor operator * (const differentiable_tensor &other) const noexcept
        {
            differentiable_tensor result(*this); // Copy the left operand
            result *= other;                     // Use *= operator
            return result;
        }
        
        ///
        /// \brief Division assignment operator
        ///
        constexpr differentiable_tensor &operator /= (const differentiable_tensor &other) noexcept
        {
            // We need to compute all derivatives before modifying value
            auto result_value = value / other.value;
            std::array<tensor_type, parameters_> result_derivatives;

            for(std::size_t i = 0; i < parameters_; ++i){
                tensor_type denominator_ = other.value * other.value;
                result_derivatives[i] = ((other.value * derivatives[i]) - (value * other.derivatives[i])) / denominator_; // Quotient rule for derivatives: d(u/v)/dx = (v * du/dx - u * dv/dx) / v^2
            }

            value = result_value;
            derivatives = result_derivatives;
            return *this;
        }

        /// 
        /// \brief Division operator
        /// 
        [[nodiscard]] constexpr differentiable_tensor operator / (const differentiable_tensor &other) const noexcept
        {
            differentiable_tensor result(*this); // Copy the left operand
            result /= other;                     // Use /= operator
            return result;
        }
       


        // Addition by element
        constexpr differentiable_tensor &operator += (const element_type &scalar) noexcept
        {
            value += scalar;
            // Derivatives remain unchanged
            return *this;
        }

        /// 
        /// \brief Scalar addition
        /// 
        [[nodiscard]] constexpr differentiable_tensor operator + (const element_type &scalar) const noexcept
        {
            differentiable_tensor result_(*this);
            result_ += scalar;
            return result_;
        }

        // Subtraction by element
        constexpr differentiable_tensor &operator -= (const element_type &scalar) noexcept
        {
            value -= scalar;
            // Derivatives remain unchanged
            return *this;
        }

        /// 
        /// \brief Scalar subtraction
        /// 
        [[nodiscard]] constexpr differentiable_tensor operator - (const element_type &scalar) const noexcept
        {
            differentiable_tensor result(*this);
            result -= scalar;
            return result;
        }

        // Multiplication by element
        constexpr differentiable_tensor &operator *= (const element_type &scalar) noexcept
        {
            value *= scalar;
            for(std::size_t i = 0; i < parameters_; ++i){
                derivatives[i] *= scalar;
            }
            return *this;
        }

        /// 
        /// \brief Scalar multiplication
        /// 
        [[nodiscard]] constexpr differentiable_tensor operator * (const element_type &scalar) const noexcept
        {
            differentiable_tensor result(*this);
            result *= scalar;
            return result;
        }

        // Division by element
        constexpr differentiable_tensor &operator /= (const element_type &scalar) noexcept
        {
            value /= scalar;
            for(std::size_t i = 0; i < parameters_; ++i){
                derivatives[i] /= scalar;
            }
            return *this;
        }

        /// 
        /// \brief Scalar division
        /// 
        [[nodiscard]] constexpr differentiable_tensor operator / (const element_type &scalar) const noexcept
        {
            differentiable_tensor result(*this);
            result /= scalar;
            return result;
        }

        // Non-member binary operators with scalar as left operand
        friend constexpr differentiable_tensor operator + (const element_type &scalar, const differentiable_tensor &tensor) noexcept
        {
            return tensor + scalar;
        }

        friend constexpr differentiable_tensor operator - (const element_type &scalar, const differentiable_tensor &tensor) noexcept
        {
            differentiable_tensor result_;
            result_.value = scalar - tensor.value;
            for(std::size_t i = 0; i < parameters_; ++i){
                result_.derivatives[i] = -tensor.derivatives[i]; // Derivatives get negated
            }
            return result_;
        }

        friend constexpr differentiable_tensor operator * (const element_type &scalar, const differentiable_tensor &tensor) noexcept
        {
            return tensor * scalar;
        }

        friend constexpr differentiable_tensor operator / (const element_type &scalar, const differentiable_tensor &tensor) noexcept
        {
            // Derivative of scalar/X is -scalar * X^(-2) * dX/dp
            differentiable_tensor result_;
            result_.value = scalar / tensor.value;

            for(std::size_t i = 0; i < parameters_; ++i){
                tensor_type denominator = tensor.value * tensor.value;
                result_.derivatives[i] = -scalar * tensor.derivatives[i] / denominator;
            }

            return result_;
        }

        /// 
        /// \brief Sum of all elements with derivatives
        /// 
        [[nodiscard]] constexpr auto sum() const noexcept
        {
            differentiable_tensor<tensor<element_type, 1>, parameters_> result_;
            result_.value(0) = value.sum();

            for(std::size_t i = 0; i < parameters_; ++i){
                result_.derivatives[i](0) = derivatives[i].sum();
            }

            return result_;
        }

        /// 
        /// \brief Mean of all elements with derivatives
        /// 
        [[nodiscard]] constexpr auto mean() const noexcept
        {
            differentiable_tensor<tensor<element_type, 1>, parameters_> result_;
            result_.value(0) = value.mean();

            for(std::size_t i = 0; i < parameters_; ++i){
                result_.derivatives[i](0) = derivatives[i].mean();
            }

            return result_;
        }

        /// 
        /// \brief Frobenius norm with derivatives
        /// 
        [[nodiscard]] constexpr auto frobenius_norm() const noexcept
        {
            // Frobenius norm: sqrt(sum(x_i^2))
            // Derivative: 1/(2*norm) * 2*x_i * dx_i/dp = x_i/norm * dx_i/dp

            differentiable_tensor<tensor<element_type, 1>, parameters_> result_;
            result_.value(0) = value.frobenius_norm();

            // Only compute the denominator once
            element_type norm_value = result_.value(0);

            for(std::size_t i = 0; i < parameters_; ++i){
                // Calculate (value * derivatives[i]).sum() / norm_value
                result_.derivatives[i](0) = (value * derivatives[i]).sum() / norm_value;
            }

            return result_;
        }

        /// 
        /// \brief Transpose the tensor and its derivatives
        /// 
        [[nodiscard]] constexpr auto transpose() const noexcept
        {
            using transposed_type = decltype(value.transpose());
            differentiable_tensor<transposed_type, parameters_> result;

            result.value = value.transpose();
            for(std::size_t i = 0; i < parameters_; ++i){
                result.derivatives[i] = derivatives[i].transpose();
            }

            return result;
        }


        /// 
        /// \brief Comparison with another differentiable tensor (value comparison only)
        /// 
        [[nodiscard]] constexpr bool operator == (const differentiable_tensor &other) const noexcept
        {
            return value == other.value;
        }

        /// 
        /// \brief Three-way comparison (value comparison only)
        /// 
        [[nodiscard]] constexpr auto operator <=> (const differentiable_tensor &other) const noexcept
        {
            return value <=> other.value;
        }
    };

    static_assert(std::is_trivially_copyable_v< differentiable_tensor<scalar<double>, 1> >);
    static_assert(std::is_standard_layout_v   < differentiable_tensor<scalar<double>, 1> >);

    ///
    /// \brief Helper alias for creating differentiable scalars
    ///
    template<typename element_t_, std::size_t parameters_>
    using differentiable_scalar = differentiable_tensor<scalar<element_t_>, parameters_>;

    ///
    /// \brief Helper alias for creating differentiable vectors
    ///
    template<typename element_t_, std::size_t size_, std::size_t parameters_>
    using differentiable_vector = differentiable_tensor<vector<element_t_, size_>, parameters_>;




} // namespace esl::mathematics

#endif // ESL_MATHEMATICS_DIFFERENTIABLE_TENSOR