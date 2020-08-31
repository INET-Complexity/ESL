/// \file   block_pool.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2014-08-27
/// \copyright  Copyright 2014-2017 Maarten P. Scholl,
///             2017-2020 The Institute for New Economic Thinking,
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
#ifndef ESL_BLOCK_POOL_HPP
#define ESL_BLOCK_POOL_HPP

#include <cstdint>
#include <utility>
#include <stdexcept>
#include <array>


namespace esl::computation::block_pool {


    ///
    /// \brief  A block containing a single value of the container.
    ///
    template<typename element_t_>
    struct block
    {
#if DEBUG
        ///
        /// \brief  Used to track overwriting/deleting non-existing items
        ///         in debug mode
        ///
        bool set;
#endif
        ///
        /// \brief  The contained value
        ///
        element_t_ data;

        typedef std::uint64_t index_t;

        ///
        /// \brief  The index associated with this block
        ///
        index_t index;

        ///
        /// \brief  A pointer to the next free block
        ///
        block *empty;
    };

    ///
    /// \brief   This memory data-structure provides constant time
    ///          insertion/deletion/indexing
    ///          while maintaining unique strictly increasing indices
    ///
    ///
    /// \tparam capacity
    /// \tparam element_t_
    template< typename element_t_
            , size_t capacity_
            , typename index_t_ = std::uint64_t
            >
    class static_block_pool
    {
    public:
        typedef element_t_ value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef element_t_ &reference;
        typedef const element_t_ &const_reference;
        typedef element_t_ *pointer;
        typedef const element_t_ *const_pointer;

        typedef index_t_ index;

    private:
        ///
        /// \brief  Underlying container
        ///
        std::array<block<element_t_>, capacity_> blocks;

        ///
        /// \brief  Points to first free element
        ///
        block<element_t_> *end;

        ///
        /// \brief  Because indices are increasing and unique, index values can
        ///         take on values (round_ * capacity_) + offset with
        ///         0 <= offset < capacity_
        ///
        index round_;

        ///
        /// \brief
        ///
        index back_;

        ///
        /// \brief  Number of active element in the container.
        ///
        size_type size_;

    public:
        static_block_pool()
        {
            for(end = &blocks.back(); end >= blocks.begin(); --end) {
                end->empty = end + 1;
            }
            end     = blocks.begin();
            round_  = 0;
            back_   = 0;
            size_   = 0;
        }

        ~static_block_pool() = default;

        size_type size()
        {
            return size_;
        }

        std::pair<index, block<element_t_> *> emplace(const element_t_ &e)
        {
            if(size_ >= capacity_) {
                throw std::length_error("block_pool container at capacity");
            }
#if DEBUG
            if(end->set) {
                throw std::logic_error("trying to insert on existing element");
            }
            end->set = true;
#endif
            auto i        = end;
            i->data       = e;
            size_t offset = i - blocks.data();

            if(back_ > offset) {
                ++round_;
                back_ = offset;
            }else{
                ++back_;
            }

            if(back_ > capacity_) {
                ++round_;
                back_ = 0;
            }

            end = end->empty;
            ++size_;

            return {round_ * capacity_ + offset, i};
        }

        ///
        /// \brief  Erase element at index i (i must be valid)
        ///
        /// \param i
        /// \return
        size_type erase(index i)
#if !DEBUG
            noexcept
#endif
        {
            block<element_t_> *removed_ = blocks.data() + (i % capacity_);

#if DEBUG
            if(!removed->set) {
                throw std::exception("trying to erase non-existing element");
            }
            removed->set = false;
#endif
            removed_->empty = end;
            end             = removed_;

            if(size_ == 0){
#if DEBUG
                throw std::exception("trying to erase non-existing element");
#else
                return 0;
#endif
            }

            --size_;
            return 1;
        }

        constexpr reference operator [] (index i)
        {
            return *(blocks + (i % capacity_));
        }

        constexpr const_reference operator [] (index i) const
        {
            return this->operator[](i);
        }

        constexpr reference at(index i)
        {
            block<element_t_> *element_ = blocks + (i % capacity_);
#if DEBUG
            if(!element_->set) {
                throw std::exception("trying to access non-existing element");
            }
#endif
            return *element_;
        }

        constexpr const_reference at(index i) const
        {
            return at(i);
        }
    };
}


#endif  // ESL_BLOCK_POOL_HPP
