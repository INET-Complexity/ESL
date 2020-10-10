/// \file   producer.hpp
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
#ifndef ESL_DATA_PRODUCER_HPP
#define ESL_DATA_PRODUCER_HPP

#include <memory>

//#include <boost/serialization/unordered_map.hpp>

#include <esl/data/output.hpp>


namespace esl::data {

    struct producer
    {
        ///
        /// \brief  The observables in (this shard of -) the simulation.
        ///
        std::unordered_map<std::string, std::shared_ptr<output_base>> outputs;

        void add_output_stream(const std::string &name, std::shared_ptr<stream> s)
        {
            auto i = outputs.find(name);
            if(outputs.end() == i){
                throw std::invalid_argument("output \"" + name + "\" not found (use create_output first)");
            }
            i->second->streams.push_back(s);
        }

        producer() = default;

        virtual ~producer() = default;

        template<typename... variable_types_>
        std::shared_ptr<output<variable_types_...>>
        create_output(const std::string &name)
        {
            auto output_ = std::make_shared<output<variable_types_...>>(name);
            outputs.insert(
                {name, std::static_pointer_cast<output_base>(output_)});
            return output_;
        }

        template<typename archive_t>
        void save(archive_t &archive, const unsigned int version) const
        {
            (void) version;
            size_t size_ = outputs.size();
            archive <<BOOST_SERIALIZATION_NVP(size_);

            for(auto [key_,v] : outputs){
                output_base &value_ = *v;
                archive << boost::serialization::make_nvp("name", key_);
                archive << boost::serialization::make_nvp<output_base>(key_.c_str(), value_);
            }
        }

        template<typename archive_t>
        void load(archive_t &archive, const unsigned int version)
        {
            (void)version;

            size_t size_ = 0;
            archive >> BOOST_SERIALIZATION_NVP(size_);

            for(size_t i = 0; i < size_; ++i){
                std::string  key_;
                archive >> boost::serialization::make_nvp("name", key_);
                output_base *value_ = nullptr;
                archive >> boost::serialization::make_nvp(key_.c_str(), value_);
                assert(value_);
            }
        }

        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;

            //archive &boost::serialization::make_nvp("outputs", outputs);

            //size_t size_ = outputs.size();
            //archive &BOOST_SERIALIZATION_NVP(size_);

            boost::serialization::split_member(archive, *this, version);
        }
    };
}  // namespace esl::data


#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::data::producer>
        : public mpl::false_
    {

    };
}  // namespace boost::mpi
#endif  // WITH_MPI


#endif  // ESL_DATA_PRODUCER_HPP
