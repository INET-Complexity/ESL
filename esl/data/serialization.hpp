/// \file   serialization.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-02-02
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
#ifndef ME_SERIALIZATION_HPP
#define ME_SERIALIZATION_HPP


#include <tuple>

#include <boost/serialization/nvp.hpp>


namespace boost::serialization {
    template<std::uint64_t element_>
    struct serializer
    {
        template<typename archive_t, typename... arguments_>
        static void serialize(archive_t &archive, std::tuple<arguments_...> &t, const unsigned int version)
        {
            archive & boost::serialization::make_nvp( ("element_" + std::to_string(element_-1)).c_str()
                                                    , std::get<element_-1>(t));
            serializer<element_-1>::serialize(archive, t, version);
        }
    };

    template<>
    struct serializer<0>
    {
        template<typename archive_t, typename... arguments_>
        static void serialize(archive_t &archive, std::tuple<arguments_...> &t, const unsigned int version)
        {
            (void) archive;
            (void) t;
            (void) version;
        }
    };

    template<typename archive_t, typename... arguments_>
    void serialize(archive_t &archive, std::tuple<arguments_...> &t, const unsigned int version)
    {
        serializer<sizeof...(arguments_)>::serialize(archive, t, version);
    }
}




#endif //ME_SERIALIZATION_HPP
