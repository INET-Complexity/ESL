/// \file   block_pool.hpp
///
/// \brief  Block memory allocators.
///
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
#include <vector>


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

        ///
        /// \brief
        ///
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
            //, size_t capacity_
            , typename index_t_ = std::uint64_t
            >
    class static_block_pool
    {
    public:
        typedef element_t_ value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;

        typedef index_t_ index;

    private:

        ///
        /// \brief  Underlying container
        ///
        std::vector<block<element_t_>> blocks;

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
        static_block_pool(size_t capacity)
        : blocks(capacity, block<element_t_>())
        {
            blocks[capacity - 1].empty = nullptr;
            for(size_t i = 1; i < capacity; ++i){
                blocks[capacity - 1 - i].empty = &blocks[capacity - i];
            }
            end     = &blocks[0];
            round_  = 0;
            back_   = 0;
            size_   = 0;
        }

        ~static_block_pool() = default;


        ///
        /// \brief current number of elements in the data structure
        ///
        /// \return
        [[nodiscard]] size_type size() const
        {
            return size_;
        }

        ///
        /// \brief  maximum number of elements that fit in the data structure
        ///
        /// \return
        [[nodiscard]] size_type capacity() const
        {
            return blocks.size();
        }

        ///
        /// \brief  Places a new element `e` into the block_pool
        ///
        /// \param e
        /// \return iterator pointing at the element
        std::pair<index, block<element_t_> *> emplace(const element_t_ &e)
        {
            if(size_ >= capacity()) {
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
            // the offset is the number of positions since the start of the pool
            size_t offset_ = i - blocks.data();

            if(back_ > offset_) {
                ++round_;
                back_ = offset_;
            }else{
                ++back_;
            }

            if(back_ > capacity()) {
                ++round_;
                back_ = 0;
            }

            end = end->empty;
            ++size_;

            return {round_ * capacity() + offset_, i};
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
            block<element_t_> *removed_ = &blocks[i % capacity()];

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
            return blocks[i % capacity()].data;
        }

        constexpr const_reference operator [] (index i) const
        {
            return this->operator[](i);
        }

        constexpr reference at(index i)
        {
            block<element_t_> *element_ = blocks[i % capacity()];
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
