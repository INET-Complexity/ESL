/// \file   representation.hpp
///
/// \brief  This file provides default string representation for common data-
///         structures, to be used primarily in debugging and status messages.
///         For rendering datastructures to common file formats for reading by
///         external programs, see the formatters in the format/ directory.
///
/// \authors    Maarten P. Scholl
/// \date       2020-04-01
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
#ifndef ME_REPRESENTATION_HPP
#define ME_REPRESENTATION_HPP

#include <map>
#include <ostream>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <list>
#include <forward_list>

#include <boost/container/flat_map.hpp>

namespace std {
    ///
    /// \tparam key_t_
    /// \tparam value_t_
    /// \tparam comparator_
    /// \tparam allocator_
    /// \param stream
    /// \param associative
    /// \return
    template<typename key_t_, typename value_t_, typename comparator_,
             typename allocator_>
    constexpr std::ostream &operator<<(
        std::ostream &stream,
        const std::map<key_t_, value_t_, comparator_, allocator_> &associative)
    {
        stream << '{';
        if(!associative.empty()) {
            auto b = associative.cbegin();
            stream << b->first << ": " << b->second;
            ++b;
            for(auto i = b; i != associative.end(); ++i) {
                stream << ", " << i->first << ": " << i->second;
            }
        }
        stream << '}';
        return stream;
    }

    ///
    /// \tparam key_t_
    /// \tparam value_t_
    /// \tparam comparator_
    /// \tparam allocator_
    /// \param stream
    /// \param associative
    /// \return
    template<typename key_t_, typename value_t_, typename comparator_,
             typename allocator_>
    constexpr std::ostream &operator<<(
        std::ostream &stream,
        const boost::container::flat_map<key_t_, value_t_, comparator_, allocator_> &associative)
    {
        stream << '{';
        if(!associative.empty()) {
            auto b = associative.cbegin();
            stream << b->first << ": " << b->second;
            ++b;
            for(auto i = b; i != associative.end(); ++i) {
                stream << ", " << i->first << ": " << i->second;
            }
        }
        stream << '}';
        return stream;
    }

    ///
    /// \tparam element_t_
    /// \param stream
    /// \param set
    /// \return
    template<typename element_t_>
    constexpr std::ostream &operator<<(std::ostream &stream,
                                       const std::set<element_t_> &set)
    {
        stream << '{';
        if(!set.empty()) {
            stream << *set.cbegin();
            for(auto i = set.cbegin() + 1; i != set.end(); ++i) {
                stream << ", " << *i;
            }
        }
        stream << '}';
        return stream;
    }


    ///
    /// \tparam key_t_
    /// \tparam value_t_
    /// \tparam hash_
    /// \tparam equality_
    /// \tparam allocator_
    /// \param stream
    /// \param associative
    /// \return
    template<typename key_t_, typename value_t_, typename hash_,
             typename equality_, typename allocator_>
    constexpr std::ostream &
    operator<<(std::ostream &stream,
               const std::unordered_map<key_t_, value_t_, hash_, equality_,
                                        allocator_> &associative)
    {
        stream << '{';
        if(!associative.empty()) {
            auto b = associative.begin();
            stream << b->first << ": " << b->second;
            std::advance(b, 1);
            for(auto i =  b; i != associative.end(); ++i) {
                stream << ", " << i->first << ": " << i->second;
            }
        }
        stream << '}';
        return stream;
    }

    ///
    /// \tparam value_t_
    /// \tparam hash_
    /// \tparam equality_
    /// \tparam allocator_
    /// \param stream
    /// \param set
    /// \return
    template<typename value_t_, typename hash_, typename equality_,
             typename allocator_>
    constexpr std::ostream &operator<<(
        std::ostream &stream,
        const std::unordered_set<value_t_, hash_, equality_, allocator_> &set)
    {
        stream << '{';
        if(!set.empty()) {
            stream << *set.cbegin();
            for(auto i = set.cbegin() + 1; i != set.end(); ++i) {
                stream << ", " << *i;
            }
        }
        stream << '}';
        return stream;
    }

    template<typename T>
    void extract_element(std::ostream &s, T v)
    {
        s << v;
    }

    template <typename T, typename...Ts>
    void extract_element(std::ostream &s, T &&first, Ts&&... rest) {
        s << first << ", ";
        extract_element(s, std::forward<Ts>(rest)...);
    }

    ///
    /// \tparam element_t_
    /// \param stream
    /// \param tuple
    /// \return
    template<typename... element_t_>
    constexpr std::ostream &operator<<(std::ostream &stream,
                                       const std::tuple<element_t_...> &tuple)
    {
        stream << '(';
        auto print_ = [&](auto... elements_) {
            return extract_element(stream, elements_...);
        };
        std::apply(print_, tuple);
        stream << ')';
        return stream;
    }


    ///
    ///  \brief  Formats vector types, into a comma separated value list
    ///          delimited by square brackets
    ///
    /// \tparam element_t_
    /// \param os
    /// \param vector
    /// \return
    template<typename element_t_>
    constexpr std::ostream &operator<<(std::ostream &stream,
                                       const std::vector<element_t_> &vector)
    {
        stream << '[';
        if(!vector.empty()) {
            stream << *vector.cbegin();
            for(auto i = vector.cbegin() + 1; i != vector.end(); ++i) {
                stream << ", " << *i;
            }
        }
        stream << ']';
        return stream;
    }

    ///
    /// \tparam element_t_
    /// \param stream
    /// \param queue
    /// \return
    template<typename element_t_>
    constexpr std::ostream &operator<<(std::ostream &stream, std::queue<element_t_> queue)
    {
        stream << '[';
        if(!queue.empty()){
            stream << queue.front();
            queue.pop();
            for(size_t c = 0; c < queue.size(); ++c){
                stream << ", " << queue.front();
                queue.pop();
            }
        }
        stream << ']';
        return stream;
    }

    ///
    ///  \brief  Formats std::forward_list, a single linked list data structure.
    ///
    /// \tparam element_t_
    /// \param os
    /// \param vector
    /// \return
    template<typename element_t_>
    constexpr std::ostream &operator<<(std::ostream &stream,
                                       const std::forward_list<element_t_> &list)
    {
        stream << '[';
        if(!list.empty()) {
            stream << *list.cbegin();
            for(auto i = list.cbegin() + 1; i != list.end(); ++i) {
                stream << " -> " << *i;
            }
        }
        stream << ']';
        return stream;
    }

    ///
    ///  \brief  Formats std::list, a doubly linked list data structure.
    ///
    /// \tparam element_t_
    /// \param os
    /// \param vector
    /// \return
    template<typename element_t_>
    constexpr std::ostream &operator<<(std::ostream &stream,
                                       const std::list<element_t_> &list)
    {
        stream << '[';
        if(!list.empty()) {
            stream << *list.cbegin();
            for(auto i = list.cbegin() + 1; i != list.end(); ++i) {
                stream << " <-> " << *i;
            }
        }
        stream << ']';
        return stream;
    }

    ///
    ///  \brief  Formats std::array, a fixed-size array
    ///
    /// \tparam element_t_
    /// \param os
    /// \param vector
    /// \return
    template<typename element_t_, size_t elements_>
    constexpr std::ostream &operator<<(std::ostream &stream,
                                       const std::array<element_t_, elements_> &
                                           array)
    {
        stream << '[';
        if(0 < elements_) {
            stream << *array.cbegin();
            for(size_t i = 1; i < elements_; ++i) {
                stream << ", " << array[i];
            }
        }
        stream << ']';
        return stream;
    }
}

#endif  // ME_REPRESENTATION_HPP
