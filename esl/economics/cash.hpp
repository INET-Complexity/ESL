/// \file   cash.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-28
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
#ifndef ESL_CASH_HPP
#define ESL_CASH_HPP

#include <esl/economics/money.hpp>
#include <esl/economics/tangibility.hpp>



namespace esl::economics {
    namespace detail {

    }


    ///
    /// \brief  (Financial) cash, that for example can be exchanged
    /// electronically. For tangible (physical) money, such as notes and coins,
    /// see physical_money
    ///
    struct cash
    : public money
    , public intangible
    {
        explicit cash(iso_4217 denomination)
        : /* law::property(esl::identity<esl::law::property>({typeid(cash).hash_code(), std::hash<esl::economics::iso_4217>()(denomination)}))
        , */ law::property(identity<law::property>({typeid(cash).hash_code(), std::hash<iso_4217>()(denomination)}))
        , money(denomination, identity<law::property>({typeid(cash).hash_code(), std::hash<iso_4217>()(denomination)}))
        {

        }

        virtual ~cash() = default;

        virtual std::string name() const  // C++20 constexpr
        {
            std::stringstream stream_;
            stream_ << denomination << " cash";
            return stream_.str();
        }

        constexpr bool operator == (const cash &c) const
        {
            return this->denomination == c.denomination;
        }

    };
}  // namespace esl::economics


namespace std {
    template<>
    struct hash<esl::economics::cash>
    {
        std::size_t operator()(const esl::economics::cash &c) const
        {
            return std::hash<esl::economics::iso_4217>()(
                c.denomination);
        }
    };
}  // namespace std

#endif  // ESL_CASH_HPP
