/// \file   identity.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-12-07
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

#ifndef ESL_SIMULATION_IDENTITY_HPP
#define ESL_SIMULATION_IDENTITY_HPP

#include <algorithm>  // TODO: use this when C++20 support is widespread
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
// using std::lexicographical_compare_3way;

#include <boost/functional/hash.hpp>
#include <boost/serialization/vector.hpp>








#ifdef WITH_PYTHON
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#endif


#include <type_traits>



namespace esl {


    ///
    /// \brief  An identifier is a code used internally to distinguish entities.
    ///         It is designed to be deterministic, so that independent runs of
    ///         the model can be more easily compared, which is done during
    ///         debugging. This is achieved by requiring that entities can only
    ///         be created by other entities, hence if external influences (e.g.
    ///         random number generators) are the same, the sequence of creation
    ///         should be the same across computing architectures, and the
    ///         resulting identifiers should be identical between runs. Then,
    ///         the identifiers form a tree-structure, where node in the tree
    ///         are the entities and the edges represent a parent-child
    ///         relationship.
    ///
    /// \tparam identifiable_type_  The type that can be uniquely identified.
    ///
    template<typename identifiable_type_>
    struct identity
    {
    public:
        ///
        /// \brief  An element of the identifier code is an unsigned 64 bits
        /// integer. This means that an entity can have
        ///         2^64 children.
        ///
        typedef std::uint64_t digit_t;

        ///
        /// \brief  The digits, elements in sequence, that make up the
        /// identifier code.
        ///
        std::vector<digit_t> digits;

        ///
        /// \param digits   vector of digits for the identifier, from most
        /// significant to least significant
        ///
        constexpr explicit identity(std::vector<digit_t> &&digits = {}) noexcept
        : digits(digits)
        {

        }

        ///
        /// \param digits   vector of digits for the identifier, from most
        /// significant to least significant
        ///
        constexpr explicit identity(const std::vector<digit_t> &digits)
        : identity(std::vector<digit_t>(digits))
        {

        }

        ///
        /// \brief  Creates identity from initializer list.
        ///
        /// \example    identity<agent> = {0,1,2};
        ///
        /// \param digits
        identity(std::initializer_list<digit_t> digits) noexcept
        : digits(digits)
        {

        }
    public:
#ifdef WITH_PYTHON

        /*
        template< typename T
                , typename = typename std::enable_if< std::is_same<T, boost::python::object>::value
                                                    || std::is_same< typename std::remove_const<typename std::remove_cv<T>::type>::type, identifiable_type_>::value
                >>
        identity(const identity<T> &i)
        : digits(i.digits)
        {

        }*/

        identity(const identity<identifiable_type_> &i)
            : digits(i.digits)
        {

        }


#else
        ///
        /// \param i    Other identity
        ///
        /// \param i
        identity(const identity<identifiable_type_> &i)
        : digits(i.digits)
        {

        }
#endif


        ///
        /// \param i    Other identity
        ///
        //identity(identity<identifiable_type_> &&i) noexcept
        //: digits(i.digits)
        //{

        //}

        ///
        /// \param rhs
        /// \return
        inline identity<identifiable_type_> &
        operator = (const identity<identifiable_type_> &rhs)
        {
            digits = rhs.digits;
            return *this;
        }

        ///
        /// \param rhs
        /// \return
        inline identity<identifiable_type_> &
        operator=(identity<identifiable_type_> &&rhs) noexcept
        {
            digits = (rhs.digits);
            return *this;
        }

        ///
        /// \param rhs
        /// \return `true` if all digits match the other identity's digits,
        ///         `false` otherwise.
        ///
        template<typename identifiable_other_type_>
        [[nodiscard]] constexpr inline bool
        operator==(const identity<identifiable_other_type_> &rhs) const
        {
            return equal(digits.begin(),
                         digits.end(),
                         rhs.digits.begin(),
                         rhs.digits.end());
        }

        ///
        /// \param rhs
        /// \return
        template<typename identifiable_other_type_>
        [[nodiscard]] constexpr inline bool
        operator!=(const identity<identifiable_other_type_> &rhs) const
        {
            return !(*this == rhs);
        }

        ///
        /// \tparam identifiable_other_type_
        /// \param rhs
        /// \return `true` if the left-hand side strictly precedes
        /// (lexicographically) the right-hand side identifier,
        ///         `false` otherwise.
        template<typename identifiable_other_type_>
        [[nodiscard]] constexpr inline bool
        operator<(const identity<identifiable_other_type_> &rhs) const
        {
            return lexicographical_compare(digits.begin(),
                                           digits.end(),
                                           rhs.digits.begin(),
                                           rhs.digits.end());
        }

        ///
        /// \todo: C++20 use lexicographical_compare_3way
        ///
        /// \param rhs
        /// \return
        template<typename identifiable_other_type_>
        [[nodiscard]] constexpr inline bool
        operator<=(const identity<identifiable_other_type_> &rhs) const
        {
            return (*this < rhs) || (*this == rhs);
        }

        ///
        /// \param rhs
        /// \return `true` if the left-hand side strictly succeeds
        /// (lexicographically) the right-hand side identifier,
        ///         `false` otherwise.
        template<typename identifiable_other_type_>
        [[nodiscard]] constexpr inline bool
        operator>(const identity<identifiable_other_type_> &rhs) const
        {
            return lexicographical_compare(rhs.digits.begin(),
                                           rhs.digits.end(),
                                           digits.begin(),
                                           digits.end());
        }

        ///
        /// TODO: C++20 use lexicographical_compare_3way
        /// \param rhs
        /// \return
        template<typename identifiable_other_type_>
        [[nodiscard]] constexpr inline bool
        operator>=(const identity<identifiable_other_type_> &rhs) const
        {
            return (*this > rhs) || (*this == rhs);
        }


        ///
        ///
        /// \param stream   The output stream to write into.
        /// \param i        The identity to render.
        ///
        /// \return     The (modified) output stream.
        friend std::ostream &operator<<(std::ostream &stream,
                                        const identity<identifiable_type_> &i)
        {
            if(i.digits.empty()) {
                return stream;
            }

            auto iterator_ = i.digits.begin();
            auto width_    = std::setw(stream.width());
            stream << std::setw(0) << '"';
            stream << std::setfill('0') << width_ << *iterator_;
            ++iterator_;
            for(; iterator_ != i.digits.end(); ++iterator_) {
                stream << '-';
                stream << std::setfill('0') << width_ << *iterator_;
            }
            stream << std::setw(0) << '"'; // stream width resets
            return stream;
        }

        ///
        /// \brief  Render the identity as a string
        ///
        /// \param width
        /// \return
        [[nodiscard]] std::string
        representation(std::streamsize width = 0) const
        {
            assert(0 <= width && width <= 20);

            std::stringstream stream_;
            stream_ << std::setw(width) << *this;
            return stream_.str();
        }

        ///
        /// \tparam child_entity_type_
        /// \tparam parent_type_
        /// \param parent
        /// \return
        template<typename child_entity_type_, typename parent_type_>
        [[nodiscard]] identity<child_entity_type_>
        create(parent_type_ &parent) const
        {
            auto prefix_ = std::vector<digit_t>(this->digits);
            prefix_.push_back(parent.children_);
            ++parent.children_;
            prefix_.shrink_to_fit();
            return identity<child_entity_type_>((prefix_));
        }

        ///
        /// \tparam archive_t
        /// \param archive
        /// \param version
        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_NVP(digits);
        }


#ifdef WITH_PYTHON
        ///
        /// \brief  This conversion is always allowed
        ///
        /// \return
        [[nodiscard]] operator identity<boost::python::object>() const
        {
            return identity<boost::python::object>(digits);
        }
#endif

        ///
        /// \brief  Up-casting implicit type conversion. Down-casting is
        /// prohibited, as this requires dynamic type
        ///         information, which is only available at the entity<> class.
        /// @tparam base_type_ a base class in the class hierarchy of
        /// identifiable_type_
        /// @return
        ///
        template<typename base_type_>
        [[nodiscard]] operator identity<base_type_>() const
        {
            static_assert(
                std::is_base_of<base_type_, identifiable_type_>::value
//#ifdef WITH_PYTON
                    || std::is_base_of<boost::python::object, identifiable_type_>::value
//#endif
                ,
                "can not cast identifier, please verify that this "
                "conversion is allowed");
            return identity<base_type_>(this->digits);
        }
    };

    ///
    /// \brief  For down-casting, we require that the user explicitly writes the
    /// cast, analogous to dynamic_pointer_cast
    /// @tparam derived_type_   The type to cast to
    /// @return                 The identity cast to the derived type
    ///
    template<typename derived_type_, typename base_type_>
    [[nodiscard]] identity<derived_type_>
    dynamic_identity_cast(const identity<base_type_> &b)
    {
        return identity<derived_type_>(b.digits);
    }

    ///
    /// \brief  Casts to the identity type ignoring class composition,
    /// potentially creating an identity that is not valid.
    ///
    /// \tparam derived_type_
    /// \tparam base_type_
    /// \param base Identity of the base class instance
    /// \return
    template<typename derived_type_, typename base_type_>
    [[nodiscard]] identity<derived_type_>
    reinterpret_identity_cast(const identity<base_type_> &base)
    {
        return identity<derived_type_>(base.digits);
    }

    ///
    /// \brief  Uses the hash of the identity's digits of the pointed-to object.
    ///
    /// \tparam identifiable_type_
    /// \tparam pointer_t_
    template<typename identifiable_type_,
             template<typename...>
             typename pointer_t_>
    struct identity_ptr_hash
    {
        [[nodiscard]] std::size_t
        operator()(pointer_t_<identifiable_type_> k) const
        {
            return std::hash<identity<identifiable_type_>>()(*k);
        }
    };

    ///
    /// \brief  Equality comparison based on the pointed-to-objects identifiers.
    ///
    /// \tparam identifiable_type_
    /// \tparam pointer_t_
    template<typename identifiable_type_,
             template<typename...>
             typename pointer_t_>
    struct identity_ptr_equality
    {
        [[nodiscard]] constexpr bool
        operator()(const pointer_t_<identifiable_type_> &lhs,
                   const pointer_t_<identifiable_type_> &rhs) const
        {
            return (*lhs) == (*rhs);
        }
    };

#ifdef WITH_PYTHON
    typedef identity<boost::python::object> python_identity;
#endif

}  // namespace esl



namespace std {
    ///
    /// \brief  Hashes an identity. Much like boost::hash_range, this combines
    /// all digits of an identity, with the difference being that the hash
    ///         of a one-digit identity is that digit itself.
    ///
    /// \tparam entity_type_
    template<typename entity_type_>
    struct hash<esl::identity<entity_type_>>
    {
        ///
        /// \param identifier
        /// \return
        [[nodiscard]] constexpr std::size_t
        operator()(const esl::identity<entity_type_> &identifier) const
        {
            if(identifier.digits.empty()) {
                return 0;
            }

            std::size_t seed_ = identifier.digits.back();
            for(auto i = identifier.digits.rbegin() + 1;
                i != identifier.digits.rend();
                ++i) {
                boost::hash_combine(seed_, *i);
            }

            return seed_;
        }
    };
}  // namespace std


#ifdef WITH_MPI
#include <boost/mpi.hpp>

namespace boost::mpi {
    ///
    /// \brief  An identifier contains only a vector<uint64_t>
    /// (MPI_UNSIGNED_LONG[]), which is a valid mpi data type.
    ///
    /// \tparam identifiable_type_
    template<typename identifiable_type_>
    struct is_mpi_datatype<esl::identity<identifiable_type_>> : mpl::true_
    {};
}  // namespace boost::mpi

#endif  // WITH_MPI

#endif  // ESL_SIMULATION_IDENTITY_HPP
