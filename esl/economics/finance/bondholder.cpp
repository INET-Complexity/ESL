/// \file   bondholder.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-01-03
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
#include <esl/economics/finance/bondholder.hpp>

#include <esl/economics/finance/bond.hpp>
#include <esl/simulation/identity.hpp>

#include <esl/economics/markets/walras/quote_message.hpp>

namespace esl::economics::finance {

    bondholder::bondholder()
        : bondholder(identity<bondholder>())
    {}

    bondholder::bondholder(const identity<bondholder> &i)
        : agent(i), owner<cash>(i), owner<bond>(i)
    {
         
        auto process_quotes_ =
            [this](std::shared_ptr<markets::walras::quote_message> m,
                   simulation::time_interval step, std::seed_seq &seed) {
                (void) seed;
                for(auto &[k, v] : m->proposed){
                    assert(std::holds_alternative<price>(v.type));
                    auto p = std::make_pair(k, std::get<price>(v.type));
                    this->bond_prices.insert(std::move(p));
                }
                return step.upper;
            };

        ESL_REGISTER_CALLBACK(markets::walras::quote_message, 0, process_quotes_, "extract bond prices from Walrasian market");
    } 


}