/// \file   environment.hpp
///
/// \brief  An environment manages computing resources when running a simulation
///         model.
///
/// \authors    Maarten P. Scholl
/// \date       2020-05-05
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
#ifndef ESL_COMPUTATION_THREAD_POOL_HPP
#define ESL_COMPUTATION_THREAD_POOL_HPP


#include <esl/computation/blocking_queue.hpp>
#include <functional>
#include <future>


namespace esl::computation {

    ///
    /// \brief  A datastructure that assigns tasks to a given number of threads.
    ///
    class thread_pool
    {
    private:
        ///
        /// \brief
        ///
        std::vector<blocking_queue<std::function<void(void)>>> queues_;

        ///
        /// \brief
        ///
        std::vector<std::thread> threads_;

        ///
        ///
        ///
        std::atomic<unsigned int> index_ = 0;

    public:
        ///
        /// \brief  The workload per thread
        ///
        constexpr static double load_factor = 2.;

        const unsigned int threads;

        explicit thread_pool(unsigned int threads = std::thread::hardware_concurrency())
        : queues_(threads)
        , threads(threads)
        {
            if (0 >= threads) {
                threads = std::thread::hardware_concurrency();
                // hardware_concurrency is not guaranteed to work well on any OS
                if (0 >= threads) {
                    threads = 1;
                }
            }

            auto worker_ = [this](auto i) {
                while (true) {
                    std::function<void(void)> function_;
                    for(unsigned int n = 0; n < (unsigned int)(this->threads * load_factor); ++n) {
                        if(queues_[(i + n) % this->threads].try_pop(function_)) {
                            break;
                        }
                    }

                    if (!function_ && !queues_[i].pop(function_)) {
                        break;
                    }

                    function_();
                }
            };

            for (unsigned int i = 0; i < threads; ++i) {
                threads_.emplace_back(worker_, i);
            }
        }

        ///
        /// \brief  Close all queues and wait for existing jobs to terminate.
        ///
        ~thread_pool()
        {
            for(auto &queue: queues_) {
                queue.close();
            }
            for(auto &thread: threads_) {
                thread.join();
            }
        }

        ///
        /// \brief  Put a function in the queue to be executed later. Do nothing
        ///         with the function's return value
        ///
        /// \tparam function_t_
        /// \tparam arguments_
        /// \param f
        /// \param a
        template<typename function_t_, typename... arguments_>
        void enqueue_work(function_t_ &&f, arguments_ &&... a)
        {
            auto work_ = [ p = std::forward<function_t_>(f)
                        , t = std::make_tuple(std::forward<arguments_>(a)...)]
            ()
            {
                std::apply(p, t);
            };

            auto i = index_++;

            for(unsigned int n = 0; n < (unsigned int)(threads * load_factor); ++n){
                if (queues_[(i + n) % threads].try_push(work_)) {
                    return;
                }
            }

            queues_[i % threads].push(std::move(work_));
        }

        ///
        /// \brief  Put a task in the queue and return a future<> which
        ///         contains the asynchronous task result
        ///
        /// \tparam function_t_
        /// \tparam arguments_
        /// \param f            task function
        /// \param a            task arguments
        /// \return
        template<typename function_t_, typename... arguments_>
        [[nodiscard]] auto enqueue_task(function_t_ &&f, arguments_ &&... a)
            -> std::future<std::invoke_result_t<function_t_, arguments_...>>
        {
            using return_type_ = std::invoke_result_t<function_t_, arguments_...>;
            auto task_ = std::make_shared<std::packaged_task<return_type_()>>
                ( std::bind(std::forward<function_t_>(f)
                , std::forward<arguments_>(a)...)
                );

            auto work = [task_]()
            {
                (*task_)();
            };

            auto result_ = task_->get_future();
            unsigned int i = index_++;

            for(auto n = 0; n < (unsigned int)(threads * load_factor); ++n){
                if(queues_[(i + n) % threads].try_push(work)){
                    return result_;
                }
            }

            queues_[i % threads].push(std::move(work));

            return result_;
        }
    };

}//esl::computation

#endif //ESL_COMPUTATION_THREAD_POOL_HPP
