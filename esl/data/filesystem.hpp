/// \file   filesystem.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2021-03-18
/// \copyright  Copyright 2017-2021 The Institute for New Economic Thinking,
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
#ifndef MARKET_ECOLOGY_FILESYSTEM_HPP
#define MARKET_ECOLOGY_FILESYSTEM_HPP

//  test <filesystem>
#if defined(__cpp_lib_filesystem)
    #define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
// test <experimental/filesystem>
#elif defined(__cpp_lib_experimental_filesystem)
    #define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
// fall back on <experimental/filesystem> if __has_include is not available
#elif !defined(__has_include)
    #define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
// Check if the header "<filesystem>" exists
#elif __has_include(<filesystem>)
    // test Visual Studio and C++17 mode
    #ifdef _MSC_VER
        #if __has_include(<yvals_core.h>)
            #include <yvals_core.h>
            #if defined(_HAS_CXX17) && _HAS_CXX17
                #define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
            #endif
        #endif
        // fall back to experimental, if we can't determine C++17
        #ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
            #define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
        #endif
    #else // #ifdef _MSC_VER
        #define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
    #endif

#elif __has_include(<experimental/filesystem>)
    #define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
#else
    #error Missing "<filesystem>" or "<experimental/filesystem>"
#endif

#if INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
    #include <experimental/filesystem>
    // pull all identifiers from the experimental:: namespace
    namespace std {
        namespace filesystem = experimental::filesystem;
    }
#else
    #include <filesystem>
#endif

#endif  // MARKET_ECOLOGY_FILESYSTEM_HPP
