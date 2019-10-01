/// \file   physical_money.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-03
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking, Oxford Martin School, University of Oxford
///
///             Licensed under the Apache License, Version 2.0 (the "License");
///             you may not use this file except in compliance with the License.
///             You may obtain a copy of the License at
///
///                 http://www.apache.org/licenses/LICENSE-2.0
///
///             Unless required by applicable law or agreed to in writing, software
///             distributed under the License is distributed on an "AS IS" BASIS,
///             WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
///             See the License for the specific language governing permissions and
///             limitations under the License.
///
///             You may obtain instructions to fulfill the attribution requirements in CITATION.cff
///

#ifndef ESL_PHYSICAL_MONEY_HPP
#define ESL_PHYSICAL_MONEY_HPP

#include <esl/economics/money.hpp>
#include <esl/economics/tangible.hpp>


namespace esl::economics {
    struct physical_money
    : public virtual money
    , public virtual tangible
    {
        physical_money(currency denomination, uint64_t quantity = 0)
        : money(denomination, quantity)
        {

        }

        virtual ~physical_money() = default;
    };

}

#endif//ESL_PHYSICAL_MONEY_HPP
