/// \file   parametrization.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-28
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
#ifndef ESL_PARAMETRIZATION_HPP
#define ESL_PARAMETRIZATION_HPP

#include <map>
#include <memory>
#include <string>

#include <esl/simulation/time.hpp>
#include <esl/simulation/parameter/parameter.hpp>


namespace esl::simulation::parameter {

    struct parametrization
    {
        std::map<std::string, std::shared_ptr<parameter_base>> values;

        ///
        /// \brief Constructs a model parametrization with the minimum number of
        ///        parameters.
        ///
        /// \param sample
        /// \param start
        /// \param end
        parametrization( std::uint64_t sample       = 0
                       , time_point start           = time_point()
                       , time_point end             = time_point() + 1
#ifdef ESL_RELEASE
                       , std::uint64_t verbosity    = 0
#else
                       , std::uint64_t verbosity    = 1
#endif
                       , unsigned int threads       = 1)

        {
            values["sample"]    = std::make_shared<constant<std::uint64_t>>(sample);
            values["start"]     = std::make_shared<constant<time_point>>(start);
            values["end"]       = std::make_shared<constant<time_point>>(end);
            values["verbosity"] = std::make_shared<constant<std::uint64_t>>(verbosity);
            values["threads"]   = std::make_shared<constant<std::uint64_t>>(verbosity);
        }

        ///
        /// \brief
        ///
        /// \tparam parameter_t_
        /// \param name
        /// \return
        template<typename parameter_t_>
        const parameter_t_ &get(const std::string &name) const
        {
            auto iterator_ = values.find(name);
            if(values.end() == iterator_) {
                throw std::out_of_range("parametrization[" + name + "] does not exist");
            }
            auto value_ =
                std::dynamic_pointer_cast<constant<parameter_t_>>(iterator_->second);
            if(!value_){
                throw std::out_of_range("parametrization[" + name + "] does not match type " + typeid(parameter_t_).name());
            }
            return value_->choice;
        }

        template<class archive_t>
        void serialize(archive_t & archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_NVP(values);
        }
    };
}

#endif  // ESL_PARAMETRIZATION_HPP
