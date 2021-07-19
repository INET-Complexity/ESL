/// \file   asset.hpp
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
#ifndef ESL_ECONOMICS_ASSET_HPP
#define ESL_ECONOMICS_ASSET_HPP

//#include <boost/serialization/serialization.hpp>

#include <esl/quantity.hpp>
#include <esl/economics/price.hpp>
#include <esl/law/property.hpp>


namespace esl {
    template<typename entity_t_>
    struct identity;
}

namespace esl::economics {

    namespace accounting {
        struct standard;
    }

    ///
    /// \brief  A property that generates value for the holder.
    ///
    /// \detail An asset is a property that can generate value, and through
    ///         analysis of the value generating process it can be valued.
    ///         Because agents are able to value it, trading or lending against
    ///         money is possible. This class mixes in fungibility, since only
    ///         fungible assets can be valued together (sum)
    ///
    struct asset
    : virtual law::property
    {
        ///
        /// \brief
        ///
        explicit asset(esl::identity<law::property> i = esl::identity<law::property>());

        virtual ~asset() = default;

        ///
        /// \brief
        ///
        [[nodiscard]] std::string name() const override;


        ///
        /// \param a    Valuation method and accounting rules
        /// \return
        //[[nodiscard]] virtual price value(const accounting::standard &a, const quantity &amount) const;




        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(law::property);
        }
    };




}  // namespace esl::economics

#endif  // ESL_ECONOMICS_ASSET_HPP
