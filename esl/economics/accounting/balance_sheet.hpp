/// \file   balance_sheet.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-01-27
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
#ifndef ME_BALANCE_SHEET_HPP
#define ME_BALANCE_SHEET_HPP


#include <esl/economics/owner.hpp>
#include <memory>

namespace esl::economics::accounting {
    class balance_sheet
    : virtual esl::law::owner<esl::law::property>
    {
    public:
        balance_sheet(esl::identity<esl::law::owner<esl::law::property>> i)
        : esl::law::owner<esl::law::property>(i)
        {

        }

        price assets(const esl::economics::accounting::standard &s) const
        {
            price result_ = price(0, s.reporting_currency);
            for(const auto &[property_, q]:  esl::law::owner<esl::law::property>::inventory){
                /// TODO: perhaps use compile time type information to filter assets
                std::shared_ptr<asset> asset_ = std::dynamic_pointer_cast<asset>(property_);
                if(asset_){
                    result_ += asset_->value(s);
                }
            }
            return result_;
        }
    };

}


#endif //ME_BALANCE_SHEET_HPP
