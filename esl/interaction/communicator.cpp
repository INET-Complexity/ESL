/// \file   communicator.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-13
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
#include <esl/interaction/communicator.hpp>
#include <esl/data/log.hpp>

#include <chrono>
using std::chrono::high_resolution_clock;

namespace esl::interaction {
    communicator::communicator(scheduling schedule)
    : locked_(false)
    , schedule_(schedule)
    {

    }

    std::map<std::string, double> timings_callback_;

    ///
    /// \brief  Handles a single message, calling all associated callbacks
    ///
    /// \param  message
    /// \param  step
    /// \param  seed
    /// \return time_point of the next event as the minimum of all future events
    ///         returned by the callback functions.
    simulation::time_point
    communicator::process_message(message_t message,
                                  simulation::time_interval step,
                                  std::seed_seq &seed) const
    {
        auto first_event_ = step.upper;

        auto callback_ = callbacks_.find(message->type);
        if(callbacks_.end() == callback_) {
            return first_event_;
        }

        for(auto i = callback_->second.rbegin(); i != callback_->second.rend();
            ++i) {
            timings_callback_.emplace(std::get<1>(*i).description, 0.);
            auto before_handler_ = high_resolution_clock::now();

            auto next_event_ = std::get<1>(*i).function(message, step, seed);
            assert(step.lower <= next_event_ && next_event_ <= step.upper);
            first_event_ = std::min(first_event_, next_event_);

            timings_callback_[std::get<1>(*i).description] += double(
                    (high_resolution_clock::now() - before_handler_).count());

//            if (step.lower % 1000 == 0) {
//                std::cout << std::get<1>(*i).description << ": "
//                          << timings_callback_[std::get<1>(*i).description] / 1e+9 / step.lower << " s" << std::endl;
//            }
        }



        return first_event_;
    }



    ///
    /// \param step
    /// \param seed
    /// \return
    simulation::time_point
    communicator::process_messages(const simulation::time_interval &step,
                                   std::seed_seq &seed)
    {
        // create mapping priority -> message



        std::multimap<priority_t, message_t, std::less<>,     boost::fast_pool_allocator<std::pair<const priority_t, message_t> >       > priority_;
        for(const auto &[k, m] : inbox) {
            // message will be received in the future
            if(k > step.lower) {
                break;
            }

            auto callback_ = callbacks_.find(m->type);
            if(callbacks_.end() == callback_ || callback_->second.empty()) {
                continue;  // no callbacks that process this message
            }

            auto highest_priority = std::numeric_limits<priority_t>::min();
            for(const auto &[p, cf] : callback_->second) {
                (void) cf;
                highest_priority = std::max(highest_priority, p);
            }

            priority_.insert(std::make_pair(highest_priority, m));
        }

        auto first_event_ = step.upper;

        for(auto i = priority_.rbegin(); i != priority_.rend(); ++i) {
            std::vector<message_t> equal_;
            for(auto upper_ = i;
                upper_ != priority_.rend() && upper_->first == i->first;
                ++upper_) {
                equal_.push_back(upper_->second);
                i = upper_;
            }

            if(random == schedule_) {
                std::minstd_rand g(seed);
                std::shuffle(equal_.begin(), equal_.end(), g);
            }

            for(const auto &m : equal_) {
                auto next_event_ = process_message(m, step, seed);
                first_event_     = std::min(first_event_, next_event_);

            }
        }
        return first_event_;
    }

    void communicator::trace_callbacks() const
    {

        for(const auto &[k, callbacks_]: callbacks_){
            if(callbacks_.empty()){
                continue;
            }
//#ifdef ESL_BUILD_RELEASE
  //          LOG(trace) << "priority " << callbacks_.begin()->first << std::endl;
    //        for(const auto &[p, f]: callbacks_){
      //          LOG(trace) << "\t[" << int(p) << "] " << f.function.target_type().name() << std::endl;
        //    }
///#else
            LOG(trace) << callbacks_.begin()->second.message << std::endl;
            for(const auto &[p, f]: callbacks_){
                LOG(trace) << "\t[" << int(p) << "] " << f.description << std::endl;
            }
//#endif
        }
    }



}  // namespace esl::interaction


#ifdef WITH_PYTHON
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
#include <boost/python/enum.hpp>
#include <esl/interaction/message.hpp>

namespace esl::interaction {
    using namespace boost::python;


    /*
    ///
    /// \brief
    ///
    /// \details    We can not deduce the message type from the callback during
    ///             runtime. For now, a simple solution is to force the Python
    ///             user to filter based on message type.
    ///
    /// \note   A more forthcoming implementation must dynamicly match Python
    ///         function signatures to the message type codes in the library.
    ///         This could mean that this function must be aware of all messages
    ///         in the library.
    ///
    /// \param self esl::interaction::communicator instance
    /// \param callback The python function to register to python messages
    /// \param priority \ref esl::interaction::communicator::register_callback
    void python_register_callback(communicator &self, object callback,
                                  communicator::priority_t priority = 0)
    {
        communicator::callback_t callback_ =
            [&callback](std::shared_ptr<header> p, simulation::time_interval s, std::seed_seq &seed) {
                auto result_ = callback.attr("__call__")(p,s,seed);
                if(result_.is_none()) {
                    return s.upper;
                }
                return (simulation::time_point)extract<simulation::time_point>(
                    result_);
            };

        self.register_callback<python_message>(callback_, priority);
    }

    BOOST_PYTHON_MODULE(communicator)
    {
        enum_<communicator::scheduling>("scheduling")
            .value("in_order", communicator::scheduling::in_order)
            .value("random", communicator::scheduling::random)
            .export_values();

        class_<communicator>("communicator")
            .def_readwrite("inbox", &communicator::inbox)
            .def_readwrite("outbox", &communicator::outbox)
            .def("register_callback", python_register_callback);
    }
     */

}  // namespace esl::interaction
#endif
