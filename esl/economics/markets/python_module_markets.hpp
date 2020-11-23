/// \file   python_module_markets.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-11-20
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
#ifndef ESL_PYTHON_MODULE_MARKETS_HPP
#define ESL_PYTHON_MODULE_MARKETS_HPP

#include <esl/economics/markets/quote_message.hpp>
#include <esl/interaction/header.hpp>

#ifdef WITH_PYTHON
namespace esl::economics::markets {
    struct python_quote_message
    : quote_message<python_quote_message, interaction::library_message_code<0x0100>()>
    {

    };

#endif

}//namespace esl::economics::markets

#endif  // ESL_PYTHON_MODULE_MARKETS_HPP
