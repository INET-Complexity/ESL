/// \file   standard.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-01
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
#ifndef ESL_STANDARD_HPP
#define ESL_STANDARD_HPP

#include <esl/economics/iso_4217.hpp>


namespace esl::economics::accounting {

    ///
    /// \brief  The accounting method wraps all parameters to value assets. For
    ///          example, model parameters for mark-to-model valuation, and
    ///         market data or calibrated structures for mark-to-market
    ///         valuation.
    ///
    struct standard
    {
        const iso_4217 reporting_currency;

        explicit standard(iso_4217 reporting_currency);

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_NVP(reporting_currency);
        }
    };
}

#endif  // ESL_STANDARD_HPP
