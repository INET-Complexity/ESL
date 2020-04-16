/// \file   security.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-03-21
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
#ifndef ESL_SECURITY_HPP
#define ESL_SECURITY_HPP


#include <esl/economics/asset.hpp>
#include <esl/economics/finance/isin.hpp>
#include <esl/economics/fungibility.hpp>

#include <esl/geography/countries.hpp>

#include <esl/law/contract.hpp>


namespace esl::economics::finance {

    ///
    /// \brief
    ///
    /// \note   necessarily fungible
    ///
    struct security
    : public asset
    , public virtual fungible
    , public law::contract
    {
        isin code;

        explicit security( identity<property> i = identity<property>()
                         , isin code            = isin(geography::countries::US)
                         , std::vector<identity<agent>> parties = {})
        : asset(i)
        , contract(parties)
        , code(code)
        {
            
        }

        virtual ~security() = default;

        std::string name() const override
        {
            std::stringstream stream_;
            stream_ << "security" << ' ' << code;
            return stream_.str();
        }

        ///
        /// \tparam archive_t
        /// \param archive
        /// \param version
        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(asset);
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(fungible);
            archive &BOOST_SERIALIZATION_NVP(code);
        }
    };
}  // namespace esl::economics::finance

#endif  // ESL_SECURITY_HPP
