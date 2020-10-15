/// \file   rational.hpp
///
/// \brief  This file extends boost::rational serialization and python
///         functionality.
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-02
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
#ifndef ESL_RATIONAL_HPP
#define ESL_RATIONAL_HPP

#pragma warning(push, 0)  // supress warnings in MSVC from external code
#include <boost/rational.hpp>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#pragma warning(pop)


namespace boost::serialization {

    template<typename archive_t, typename T>
    void save(archive_t &archive, boost::rational<T> const &r,
              unsigned version)
    {
        (void)version;
        int n = r.numerator(), d = r.denominator();
        archive &boost::serialization::make_nvp("numerator", n);
        archive &boost::serialization::make_nvp("denominator", d);
    }

    template<typename archive_t, typename T>
    void load(archive_t &archive, boost::rational<T> &r, unsigned version)
    {
        (void)version;
        int n, d;
        archive &boost::serialization::make_nvp("numerator", n);
        archive &boost::serialization::make_nvp("denominator", d);
        r = boost::rational<T>(n, d);
    }
}

BOOST_SERIALIZATION_SPLIT_FREE(boost::rational<std::int64_t>);
BOOST_SERIALIZATION_SPLIT_FREE(boost::rational<std::uint64_t>);


#endif  // ESL_RATIONAL_HPP
