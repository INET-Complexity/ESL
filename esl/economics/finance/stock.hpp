/// \file   stock.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-03-21
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
#ifndef ESL_STOCK_HPP
#define ESL_STOCK_HPP


#include <memory>

#include <esl/economics/finance/security.hpp>
#include <esl/economics/finance/share_class.hpp>

namespace esl::economics {
    struct company;
}

namespace esl::economics::finance {
    struct stock
    : public virtual security
    {
        ///
        /// \brief  The company that issued the shares
        ///
        identity<company> company_identifier;

        ///
        /// \brief The description of the share
        ///
        share_class details;

        ///
        ///
        ///
        stock();

        ///
        /// \brief  Creates a fictional ISIN code for the company
        ///         This modifies the issuing company, as it needs to create a new
        ///         child property.
        ///
        stock(company &issuer, const share_class &details);


        ///
        /// \brief  Creates a fictional ISIN code for the company
        ///
        stock(const identity<property> &pi, geography::iso_3166_1_alpha_2 c,
              const identity<company> &i, const share_class &s);

        ///
        ///
        ///
        stock(const identity<property> &pi,
              const identity<company> &company_identifier,
              const share_class &details,
              const isin &code);

        //price value(const accounting::standard &a,
        //            const quantity &amount) const override;


        virtual ~stock() = default;

        /*C++20 constexpr*/ std::string name() const override
        {
            std::stringstream stream_;
            stream_ << "stock" << ' ' << this->identifier;
            return stream_.str();
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(security);
            archive &BOOST_SERIALIZATION_NVP(company_identifier);
            archive &BOOST_SERIALIZATION_NVP(details);
        }
    };

}

#endif//ESL_STOCK_HPP
