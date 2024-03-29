/// \file   currency.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-12-13
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
#ifndef ESL_CURRENCY_HPP
#define ESL_CURRENCY_HPP


namespace esl::economics {

    class currency {
        virtual ~currency() = default;
    };

}

#endif //ESL_CURRENCY_HPP
