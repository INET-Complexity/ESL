/// \file   securities_lending_contract.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-04-10
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


#ifndef ME_SECURITIES_LENDING_CONTRACT_HPP
#define ME_SECURITIES_LENDING_CONTRACT_HPP

#include <esl/economics/finance/loan.hpp>
#include <esl/quantity.hpp>


namespace esl::economics::finance {

     inline identity<law::property> prepend(identity<law::property>::digit_t v, identity<law::property> p)
    {
        std::vector<identity<law::property>::digit_t> result_ = {v};
        for(auto d: p.digits){
            result_.emplace_back(d);
        }
        return identity<law::property>(result_);
    }


    class securities_lending_contract
    : public loan
    {
    public:
        //std::map<identity<property>, quantity> basket;

        identity<property> security;
        quantity size;

        securities_lending_contract( identity<agent> lender
                                   , identity<agent> borrower
                                   , identity<property> security
                                   , quantity size)
        : property(prepend(typeid(securities_lending_contract).hash_code(), security))
        , loan(lender, borrower)
        , security(security)
        , size(size)
        {

        }

        [[nodiscard]] std::string name() const override
        {
            std::stringstream stream_;
            stream_ << "securities lending contract " << identifier;
            return stream_.str();
        }
    };

}//namespace esl::economics::finance

#endif  // ME_SECURITIES_LENDING_CONTRACT_HPP
