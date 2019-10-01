/// \file   commodity_money.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-03
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
#ifndef ESL_COMMODITY_MONEY_HPP
#define ESL_COMMODITY_MONEY_HPP

#include <esl/economics/commodity.hpp>
#include <esl/economics/physical_money.hpp>


namespace esl::economics {

    struct commodity_money
    : virtual commodity
    , virtual physical_money
    {

    };

}

#endif//ESL_COMMODITY_MONEY_HPP
