/// \file   auctioneer.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-07-19
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
#ifndef ESL_MARKET_CLEARING_PRICES_HPP
#define ESL_MARKET_CLEARING_PRICES_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>

#include <stan/services/optimize/lbfgs.hpp>

#include <esl/economics/markets/market.hpp>
#include <esl/economics/markets/walras/differentiable_order_message.hpp>

namespace esl::economics::markets::walras {

    ///
    /// \brief  "auctioneer" is the market mechanism that implements demand and supply matching via numerical method
    ///
    struct price_setter
    : market
    {
    private:
    public:
        law::property_map<quote> traded_properties; // move the market base?

        std::shared_ptr<data::output<std::vector<price>>> output_clearing_prices_;

    protected:
        friend class boost::serialization::access;

        explicit price_setter();

    public:
        enum state_t
        { sending_quotes
        , clearing_market
        };

        state_t state;

        ///
        ///
        /// \details    Initialises the differentiable variable context to 1.0 times the initial quotes. In essence, the
        ///             solver starts at 1.0 times the initial quote
        /// \param i
        /// \param traded_properties
        explicit price_setter(const identity<price_setter> &i, law::property_map<quote> traded_properties = {});

        ///
        /// \brief
        ///
        /// \param start
        /// \return
        simulation::time_point act(simulation::time_interval step,
                       std::seed_seq &seed) override;

        ///
        /// \brief
        ///
        std::map<esl::identity<esl::law::property>, double> clear_market( const std::unordered_map<identity<agent>
                                   , std::shared_ptr<walras::differentiable_order_message>> &o
                                   , const esl::simulation::time_interval &step);


        template<class archive_t>
        void serialize(archive_t & archive, const unsigned int version)
        {
            (void)version;
            archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(market);


            archive & BOOST_SERIALIZATION_NVP(traded_properties);
            archive & BOOST_SERIALIZATION_NVP(output_clearing_prices_);

            archive & BOOST_SERIALIZATION_NVP(state);
        }
    };

}//namespace walras


#ifdef WITH_MPI
#include <boost/mpi.hpp>
namespace boost {
    namespace mpi {
        template<>
        struct is_mpi_datatype<esl::economics::markets::walras::price_setter>
        : public mpl::false_
        {};

    }//namespace mpi
}//namespace boost
#endif//WITH_MPI






#endif//ESL_MARKET_CLEARING_PRICES_HPP
