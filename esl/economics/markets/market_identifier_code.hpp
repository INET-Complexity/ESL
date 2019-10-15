/// \file   security_market_identifier_code.hpp
///
/// \brief  The MIC (ISO 10383) uniquely identifies a market; specifically, identify securities trading exchanges, not
///         general markets
///
/// \authors    Maarten P. Scholl
/// \date       2019-04-10
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
#ifndef ESL_SECURITY_MARKET_IDENTIFIER_CODE_HPP
#define ESL_SECURITY_MARKET_IDENTIFIER_CODE_HPP

#include <array>
using std::array;


///
/// \brief  Defines an ISO 10383 market identifier codes
///
struct market_identifier_code
{
    const array<char, 4> code;
};

#endif//ESL_SECURITY_MARKET_IDENTIFIER_CODE_HPP
