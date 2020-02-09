/// \file   property_collection.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-09-26
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
#ifndef ESL_LAW_PROPERTY_COLLECTION_HPP
#define ESL_LAW_PROPERTY_COLLECTION_HPP

#include <unordered_map>
#include <unordered_set>

#include <map>

#include <esl/law/property.hpp>




namespace esl::law {

    template<typename property_t_>
    struct property_collection_hash
    {
        size_t operator()(const std::shared_ptr<property_t_> &k) const
        {
            return std::hash<esl::identity<property>>()(*k);
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)archive;
            (void)version;
        }
    };

    template<typename property_t_>
    struct property_collection_equality
    {
        constexpr bool operator()(const std::shared_ptr<property_t_> &lhs,
                                  const std::shared_ptr<property_t_> &rhs) const
        {
            return (*lhs) == (*rhs);
        }
    };

    using property_set = std::unordered_set<std::shared_ptr<property>,
                               property_collection_hash<property>,
                               property_collection_equality<property>>;

    template<typename value_t_>
    using property_map = std::map<std::shared_ptr<property>, value_t_>;
            /*
        std::unordered_map<std::shared_ptr<property>,
                           value_t_,
                           property_collection_hash<property>,
                           property_collection_equality<property>>;
            */

    template<typename property_t_>
    using property_filter_set =
        std::unordered_set<std::shared_ptr<property_t_>,
                           property_collection_hash<property_t_>,
                           property_collection_equality<property_t_>>;

    template<typename property_t_, typename value_t_>
    using property_filter_map =
        std::unordered_map<std::shared_ptr<property_t_>,
                           value_t_,
                           property_collection_hash<property_t_>,
                           property_collection_equality<property_t_>>;

}  // namespace esl::law

#endif  // ESL_LAW_PROPERTY_COLLECTION_HPP
