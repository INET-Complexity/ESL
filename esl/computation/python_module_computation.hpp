///// \file   python_module.hpp
/////
///// \brief
/////
///// \authors    maarten
///// \date       2020-08-30
///// \copyright  Copyright 2017-2020 The Institute for New Economic Thinking,
/////             Oxford Martin School, University of Oxford
/////
/////             Licensed under the Apache License, Version 2.0 (the "License");
/////             you may not use this file except in compliance with the License.
/////             You may obtain a copy of the License at
/////
/////                 http://www.apache.org/licenses/LICENSE-2.0
/////
/////             Unless required by applicable law or agreed to in writing,
/////             software distributed under the License is distributed on an "AS
/////             IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
/////             express or implied. See the License for the specific language
/////             governing permissions and limitations under the License.
/////
/////             You may obtain instructions to fulfill the attribution
/////             requirements in CITATION.cff
/////
//#ifndef ESL_COMPUTATION_PYTHON_MODULE_HPP
//#define ESL_COMPUTATION_PYTHON_MODULE_HPP
//
//#ifdef WITH_PYTHON
//
//#include <esl/computation/environment.hpp>
//
//namespace esl::computation {
//
//    ///
//    /// \brief  This class enables access to the computation::environment
//    ///         internal functions for Python derived classes.
//    ///
//    /// \details    Python does not have access via inheritance of protected
//    ///             members, so we need to make everything public
//    ///
//    class python_environment
//    : public esl::computation::environment
//    {
//    public:
//        python_environment()
//        : esl::computation::environment()
//        {
//
//        }
//
//        virtual ~python_environment() = default;
//
//        void step(simulation::model &model) override
//        {
//            environment::step(model);
//        }
//
//        void run(simulation::model &simulation) override
//        {
//            environment::run(simulation);
//        }
//
//        size_t activate() override
//        {
//            return environment::activate();
//        }
//
//        size_t deactivate() override
//        {
//            return environment::deactivate();
//        }
//
//        void before_step() override
//        {
//            environment::before_step();
//        }
//
//        void after_step(simulation::model &simulation) override
//        {
//            environment::after_step(simulation);
//        }
//
//        void after_run(simulation::model &simulation) override
//        {
//            environment::after_run(simulation);
//        }
//
//        void activate_agent(const identity<agent> &a) override
//        {
//            environment::activate_agent(a);
//        }
//
//        void deactivate_agent(const identity<agent> &a) override
//        {
//            environment::deactivate_agent(a);
//        }
//
//        size_t send_messages(simulation::model &simulation) override
//        {
//            return environment::send_messages(simulation);
//        }
//    };
//
//
//}//namespace esl::computation
//#endif
//
//#endif  // ESL_COMPUTATION_PYTHON_MODULE_HPP
