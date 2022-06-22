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
        index_t index = 0;

        ///
        /// \brief  A pointer to the next free block
        ///
        block *next = nullptr;
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
        /// \brief  Underlying container. Needs to be array like (not necessarily dynamically allocated and resizable)
        ///
        std::vector<block<element_t_>> blocks;

        ///
        /// \brief  Points to first free element
        ///
        block<element_t_> *begin_;

        ///
        /// \brief  Points to first free element
        ///
        block<element_t_> *end_;

        ///
        /// \brief  Because indices are increasing and unique, index values can
        ///         take on values (round_ * capacity_) + offset with
        ///         0 <= offset < capacity_
        ///
        index round_;

        ///
        /// \brief  Index of the element at the back of the pool
        ///
        index back_;

        
        ///
        /// \brief  The highest index *not yet* assigned - to ensure strictly increasing indices awarded
        ///         after cancellations
        ///
        index highest_;

        ///
        /// \brief  Number of active element in the container.
        ///
        size_type size_;

    public:
        static_block_pool(size_t capacity)
        : blocks(capacity, block<element_t_>())
        , round_(0)
        , back_(0)
        , size_(0)
        , highest_(0)
        {
            blocks[capacity - 1].next = nullptr;
            for(size_t i = 1; i < capacity; ++i){
                blocks[capacity - 1 - i].next = &blocks[capacity - i];
            }
            begin_  = &blocks[0];
            end_    = &blocks[0];
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
        /// \brief  Returns an iterator to the first element in the pool
        /// \todo implement iterator class, currently not a valid iterator
        ///
        /// \return
        block<element_t_> *begin()
        {
            return begin_;
        }

        const block<element_t_> *begin() const
        {
            return begin_;
        }

        ///
        /// \brief  Returns an iterator that denotes the end of the container,
        ///         in essence the first invalid element
        /// \todo implement iterator class, currently not a valid iterator
        ///
        /// \return
        block<element_t_> *end()
        {
            return end_;
        }

        const block<element_t_> *end() const
        {
            return end_;
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
            auto i        = end_;
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

            end_ = end_->next;
            ++size_;

            // if the offset has decreased, for because of erase()'d elements
            index assigned_ = round_ * capacity() + offset_;
            if(assigned_ < highest_) {
                ++round_;
                assigned_ = round_ * capacity() + offset_;
                
            }
            highest_ = std::max(highest_, assigned_ + 1);

            return {assigned_, i};
        }

        ///
        /// \brief  Erase element at index i (i must be valid)
        ///
        /// \param i
        /// \return 0 if no element was deleted (because the index was invalid/not in use), or 1 if the element with the index was located and removed.
        size_type erase(index i)
#if !DEBUG
            noexcept
#endif
        {
            block<element_t_> *removed_ = &blocks[i % capacity()];
            
            if(removed_->index != i) {
                return 0;
                //throw esl::exception("trying to delete stale order");
            }

#if DEBUG
            if(!removed->set) {
                throw std::exception("trying to erase non-existing element");
            }
            removed->set = false;
#endif
            // we removed one element, so if this was the first in the list,
            // we need to move the begin pointer
            if(removed_ == begin_){
                begin_ = removed_ ->next;
            }

            // ->next points to the next *free* element, so we need to add 
            // the cleared element to the start of the list of free elements
            removed_->next = end_;
            end_           = removed_;

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

        ///
        /// \brief  Returns a reference to the element at index i modulo the pool size.
        /// 
        constexpr reference operator [] (index i)
        {
            //assert(blocks[i % capacity()].index == i);
            return blocks[i % capacity()].data;
        }

        ///
        /// \brief  Returns a const reference to the element at index i.
        /// 
        constexpr const_reference operator [] (index i) const
        {
            return this->operator[](i);
        }

        constexpr reference at(index i)
        {
            assert(blocks[i % capacity()].index == i);
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
