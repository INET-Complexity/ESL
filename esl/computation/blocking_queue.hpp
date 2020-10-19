/// \file   blocking_queue.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-10-17
/// \copyright  Copyright 2017-2020 The Institute for New Economic Thinking,
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
#ifndef ESL_BLOCKING_element_t_UEUE_HPP
#define ESL_BLOCKING_element_t_UEUE_HPP

#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>


namespace esl::computation {


    template<typename element_t_>
    class blocking_queue
    {
    private:
        ///
        ///
        ///
        std::queue<element_t_> queue_;

        ///
        ///
        ///
        mutable std::mutex mutex_;

        ///
        /// \brief  Used to signal producers and consumers
        ///
        std::condition_variable variable_;

        ///
        ///
        ///
        std::atomic<bool> closed_;

    public:
        blocking_queue()
        : closed_(false)
        {

        }

        typename std::enable_if<std::is_copy_constructible<element_t_>::value, void>::type
        push(const element_t_ &item)
        {
            {
                std::unique_lock lock(mutex_);
                queue_.push(item);
            }
            variable_.notify_one();
        }

        
        typename std::enable_if<std::is_move_constructible<element_t_>::value, void>::type
        push(element_t_ &&item)
        {
            {
                std::unique_lock lock(mutex_);
                queue_.emplace(std::forward<element_t_>(item));
            }
            variable_.notify_one();
        }

        
        typename std::enable_if<std::is_copy_constructible<element_t_>::value, bool>::type
        try_push(const element_t_ &item)
        {
            {
                std::unique_lock lock(mutex_, std::try_to_lock);
                if(!lock) {
                    return false;
                }
                queue_.push(item);
            }
            variable_.notify_one();
            return true;
        }

        
        typename std::enable_if<std::is_move_constructible<element_t_>::value,
                                bool>::type
        try_push(element_t_ &&item)
        {
            {
                std::unique_lock lock(mutex_, std::try_to_lock);
                if(!lock) {
                    return false;
                }
                queue_.emplace(std::forward<element_t_>(item));
            }
            variable_.notify_one();
            return true;
        }


        template<typename specialization_ = element_t_>
        typename std::enable_if<std::is_copy_assignable<specialization_>::value
                                    && !std::is_move_assignable<specialization_>::value,
                                bool>::type
        pop(element_t_ &item)
        {
            std::unique_lock lock(mutex_);
            while(queue_.empty() && !closed_) {
                variable_.wait(lock);
            }
            if(queue_.empty()) {
                return false;
            }
            item = queue_.front();
            queue_.pop();
            return true;
        }

        typename std::enable_if<std::is_move_assignable<element_t_>::value, bool>::type
        pop(element_t_ &item)
        {
            std::unique_lock lock(mutex_);
            while(queue_.empty() && !closed_) {
                variable_.wait(lock);
            }
            if(queue_.empty()) {
                return false;
            }
            item = std::move(queue_.front());
            queue_.pop();
            return true;
        }


        template<typename specialization_ = element_t_>
        typename std::enable_if<std::is_copy_assignable<specialization_>::value
                                    && !std::is_move_assignable<specialization_>::value,
                                bool>::type
        try_pop(element_t_ &item)
        {
            std::unique_lock lock_(mutex_, std::try_to_lock);
            if(!lock_ || queue_.empty()) {
                return false;
            }
            item = queue_.front();
            queue_.pop();
            return true;
        }


        template<typename specialization_ = element_t_>
        typename std::enable_if<std::is_move_assignable<element_t_>::value, bool>::type
        try_pop(element_t_ &item)
        {
            std::unique_lock lock_(mutex_, std::try_to_lock);
            if(!lock_ || queue_.empty()) {
                return false;
            }
            item = std::move(queue_.front());
            queue_.pop();
            return true;
        }

        void close() noexcept
        {
            {
                std::unique_lock lock(mutex_);
                closed_ = true;
            }
            variable_.notify_all();
        }

        bool empty() const noexcept
        {
            std::scoped_lock lock(mutex_);
            return queue_.empty();
        }

        unsigned int size() const noexcept
        {
            std::scoped_lock lock(mutex_);
            return queue_.size();
        }
    };

}//namespace esl::computation

#endif  // ESL_BLOCKING_QUEUE_HPP
