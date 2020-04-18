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
#include <esl/economics/money.hpp>


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
            return lhs->identifier == rhs->identifier;
        }
    };

    using property_set = std::unordered_set<std::shared_ptr<property>,
                               property_collection_hash<property>,
                               property_collection_equality<property>>;

    template<typename value_t_>
    using property_map = //std::map<std::shared_ptr<property>, value_t_>;

        std::unordered_map<std::shared_ptr<property>,
                           value_t_,
                           property_collection_hash<property>,
                           property_collection_equality<property>>;



    ///
    /// \brief This is a quick hack to format properties better.
    ///         TODO consider making property_map<quantity> a proper class
    ///
    /// \tparam value_t_
    /// \tparam allocator_
    /// \param stream
    /// \param associative
    /// \return
    template<typename value_t_, typename allocator_>
    constexpr std::ostream &
    operator<<(std::ostream &stream,
               const std::unordered_map<std::shared_ptr<property>,
                   value_t_,
                   property_collection_hash<property>,
                   property_collection_equality<property>,
                   allocator_> &associative)
    {
        stream << '{';
        if(!associative.empty()) {
            auto b = associative.begin();

            std::shared_ptr<economics::money> m_ = std::dynamic_pointer_cast<economics::money>(b->first);
            if(!m_){
                stream << b->first->name()   << ": " << (b->second);
            }else{
                stream << m_->price(b->second );
            }
            std::advance(b, 1);
            for(auto i =  b; i != associative.end(); ++i) {
                std::shared_ptr<economics::money> m = std::dynamic_pointer_cast<economics::money>(i->first);
                if(!m_){
                    stream << ", " << i->first->name()   << ": " << (i->second);
                }else{
                    stream << ", " << m_->price(b->second );
                }
            }
        }
        stream << '}';
        return stream;
    }






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
