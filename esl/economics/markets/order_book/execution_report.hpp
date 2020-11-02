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
        enum state_t
        {
            invalid,
            cancel,
            match,
            placement
        } state : 2;

        std::uint32_t quantity;
        std::uint64_t identifier;
        limit_order_message::side_t side;
        quote limit;
        identity<agent> owner;

        [[nodiscard]] std::string representation() const
        {
            std::stringstream stream_;
            stream_ << (*this);
            return stream_.str();
        }

        friend std::ostream &operator<<(std::ostream &stream,
                                        const execution_report &e)
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
            }
            stream << " " << e.owner << " " << e.quantity << "@"
                   << double(e.limit) * e.limit.lot;

            return stream;
        }
    };

} // namespace esl::economics::markets::order_book

#endif  // ESL_EXECUTION_REPORT_HPP
