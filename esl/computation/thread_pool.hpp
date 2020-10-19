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
    class thread_pool
    {
    private:
        using Proc = std::function<void(void)>;
        using Queue = blocking_queue<Proc>;
        using Queues = std::vector<Queue>;
        Queues m_queues;

        using Threads = std::vector<std::thread>;
        Threads m_threads;

        const unsigned int threads_;
        std::atomic_uint m_index = 0;

        inline static const unsigned int K = 2;


    public:
        explicit thread_pool(unsigned int threads = std::thread::hardware_concurrency())
                : m_queues(threads), threads_(threads) {
            if (0 >= threads) {
                throw std::invalid_argument("Invalid thread count!");
            }

            auto worker_ = [this](auto i) {
                while (true) {
                    Proc f;
                    for (unsigned int n = 0; n < threads_ * K; ++n)
                        if (m_queues[(i + n) % threads_].try_pop(f))
                            break;
                    if (!f && !m_queues[i].pop(f))
                        break;
                    f();
                }
            };

            for (unsigned int i = 0; i < threads; ++i) {
                m_threads.emplace_back(worker_, i);
            }
        }

        ~thread_pool() {
            for (auto &queue : m_queues) {
                queue.close();
            }
            for (auto &thread : m_threads) {
                thread.join();
            }
        }

        template<typename F, typename... Args>
        void enqueue_work(F &&f, Args &&... args) {
            auto work = [p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() {
                std::apply(p, t);
            };

            auto i = m_index++;

            for (unsigned int n = 0; n < threads_ * K; ++n) {
                if (m_queues[(i + n) % threads_].try_push(work)) {
                    return;
                }
            }

            m_queues[i % threads_].push(std::move(work));
        }

        template<typename F, typename... Args>
        [[nodiscard]] auto enqueue_task(F &&f, Args &&... args) -> std::future<std::invoke_result_t<F, Args...>> {
            using task_return_type = std::invoke_result_t<F, Args...>;
            using task_type = std::packaged_task<task_return_type()>;

            auto task = std::make_shared<task_type>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            auto work = [task]() { (*task)(); };
            auto result = task->get_future();
            auto i = m_index++;

            for (auto n = 0; n < threads_ * K; ++n) {
                if (m_queues[(i + n) % threads_].try_push(work)) {
                    return result;
                }
            }

            m_queues[i % threads_].push(std::move(work));

            return result;
        }
    };

}//esl::computation

#endif //ESL_COMPUTATION_THREAD_POOL_HPP
