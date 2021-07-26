/// \file   execution_report.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-11-01
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
#ifndef ESL_EXECUTION_REPORT_HPP
#define ESL_EXECUTION_REPORT_HPP

#include <cstdint>
#include <esl/economics/markets/order_book/order.hpp>

namespace esl::economics::markets::order_book {

    struct execution_report
    {
        ///
        /// \brief
        ///
        enum state_t
        { invalid
        , cancel
        , match
        , placement
        } state;

        ///
        /// \brief  Buy or sell
        ///
        limit_order_message::side_t side;

        ///
        /// \brief  Because a report can apply to only a part of the order,
        ///         e.g. match in part, we specify the amount again.
        ///
        std::uint32_t quantity;

        ///
        /// \brief
        ///
        std::uint64_t identifier;

        ///
        /// \brief  We specify the limit again, because it is possible that
        ///         the order is match at a better price than the limit.
        ///
        quote limit;

        ///
        /// \brief
        ///
        identity<agent> owner;


        execution_report( state_t state
                        , limit_order_message::side_t side
                        , std::uint32_t quantity
                        , std::uint64_t identifier
                        , const quote& limit
                        , const identity<agent>& owner
                        )
        : state(state)
        , side(side)
        , quantity(quantity)
        , identifier(identifier)
        , limit(limit)
        , owner(owner)
        {

        }

        execution_report(const execution_report &r)
        : state(r.state)
        , side(r.side)
        , quantity(r.quantity)
        , identifier(r.identifier)
        , limit(r.limit)
        , owner(r.owner)
        {

        }


        execution_report &operator = (const execution_report &r)
        {
            state = r.state;
            side = r.side;
            quantity = r.quantity;
            identifier = r.identifier;
            limit = r.limit;
            owner = r.owner;
            return *this;
        }

        ///
        /// \brief  Modeller friendly string representation of the report
        /// \return
        [[nodiscard]] std::string representation() const
        {
            std::stringstream stream_;
            stream_ << (*this);
            return stream_.str();
        }

        ///
        /// \brief
        ///
        /// \details    Required for storage in certain datastructures.
        ///
        /// \param
        /// \return
        [[nodiscard]] constexpr bool operator == (const execution_report &o) const
        {
            return state == o.state
                   && quantity == o.quantity
                   && identifier == o.identifier
                   && side == o.side
                   && limit == o.limit
                   && owner == o.owner;
        }

        friend std::ostream &operator << ( std::ostream &stream
                                         , const execution_report &e)
        {
            switch(e.state) {
            case invalid:
                stream << "invalid";
                break;
            case cancel:
                stream << "cancel";
                break;
            case match:
                stream << "match";
                break;
            case placement:
                stream << "placement";
                break;
            default:
                throw esl::exception("invalid execution_report state");
            }

            auto quote_ = quote(e.limit);
            quote_.lot *= e.quantity;

            stream << " " << e.owner
                   << " " << e.limit;

            return stream;
        }
    };

} // namespace esl::economics::markets::order_book

#endif  // ESL_EXECUTION_REPORT_HPP
