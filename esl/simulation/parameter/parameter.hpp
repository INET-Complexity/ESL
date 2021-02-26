/// \file   parameter.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-11-19
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
#ifndef ESL_PARAMETER_HPP
#define ESL_PARAMETER_HPP

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#include <vector>

#include <esl/mathematics/interval.hpp>

namespace esl::simulation::parameter {
    struct parameter_base
    {
        virtual ~parameter_base() = default;

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            (void)archive;
        }
    };

    template<typename parameter_t_>
    struct constant
    : public parameter_base
    {
        parameter_t_ choice;

        constexpr constant(parameter_t_ choice)
        : choice(choice)
        {

        }

        constexpr explicit operator parameter_t_() const
        {
            return choice;
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(parameter_base);
            archive &BOOST_SERIALIZATION_NVP(choice);
        }
    };


    template<typename parameter_t_>
    struct categorical
    : public parameter_base
    {
        std::vector<parameter_t_> categories;

        constexpr categorical(const std::vector<parameter_t_> &categories)
        : categories(categories)
        {

        }

        constexpr parameter_t_ operator [] (size_t index) const
        {
            return categories[index];
        }


        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(parameter_base);
            archive &BOOST_SERIALIZATION_NVP(categories);
        }
    };


    template<typename parameter_t_>
    struct interval
    : public parameter_base
    , public mathematics::interval<parameter_t_, true, true>
    {
        interval(parameter_t_ lower, parameter_t_ upper)
        : parameter_base()
        , mathematics::interval<parameter_t_, true, true>(lower, upper)
        {

        }
    };

}


#endif  // ESL_PARAMETER_HPP
