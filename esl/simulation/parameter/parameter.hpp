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

        constexpr constant(parameter_t_ choice) : choice(choice)
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
}


#endif  // ESL_PARAMETER_HPP
