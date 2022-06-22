/// \file   openbook.hpp
///
/// \brief  This file provides default string representation for common data-
///         structures, to be used primarily in debugging and status messages.
///         For rendering datastructures to common file formats for reading by
///         external programs, see the formatters in the format/ directory.
///
/// \authors    Maarten P. Scholl
/// \date       2022-01-01
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
#ifndef ESL_OPENBOOK_HPP
#define ESL_OPENBOOK_HPP

#include <boost/algorithm/string.hpp>



namespace esl::data {
    template<typename binary_t_, typename buffer_t_ = char>
    binary_t_ &extract_e(buffer_t_ *intrusive)
    {
        if constexpr(std::endian::native == std::endian::little) {
            buffer_t_ *s = intrusive;
            buffer_t_ *e = intrusive + sizeof(binary_t_) - 1;

            for(s, e; s < e; ++s, --e) {
                std::swap(*s, *e);
            }
        }

        return *(reinterpret_cast<binary_t_ *>(intrusive));
    }


    ///
    /// \brief VERSION 1.2b 2017-04-20 and after
    ///
    // template<unsigned int major_version = 1, unsigned int minor_version = 2>
    void read_openbook(
        const std::string &filename,
        std::function<void(const std::string &, char, char, time_point,
                           const quote &, const std::uint32_t &,
                           limit_order::side_t)>
            callback)
    {
        std::ifstream input(filename, std::ios::binary);
        std::streampos fsize = input.tellg();

        input.seekg(0, std::ios::end);
        fsize = input.tellg() - fsize;
        input.seekg(0, std::ios::beg);

        if(0 != (fsize % 71)) {
            std::cout
                << "warning: file does not contain a multiple of 71 bytes "
                   "which is the message size"
                << std::endl;
        }

        const size_t events = fsize / 71;

        // std::ofstream csv("out.csv");
        // csv <<
        // "symbol,market,status,time,microseconds,price,denominator,size,side"
        // << std::endl;


        // FILE FORMAT
        //
        // All TAQ NYSE OpenBook Ultra data files are delivered in fixed-length
        // price point format and are compressed using GZIP. Each message is S
        // bytes in length. There are no delimiters and so the format is <S
        // bytes fields><S bytes fields>……<S bytes fields>
        //
        // Old versions have S=69 bytes.
        // Post version 1.2b there are S=71 bytes.
        //

        std::array<char, 71> buffer;


        for(int i = 0; i < events; i++) {
            input.read(&buffer[0], buffer.size());

            // +0
            // auto &MsgSeqNum = extract_e<std::uint32_t>(&buffer[0]);

            // +4
            auto &MsgType = extract_e<std::uint16_t>(&buffer[4]);
            switch(MsgType) {
            case 1:   //  1 – Sequence Number Reset
            case 2:   //  2 – Heartbeat Message
            case 5:   //  5 – Message Unavailable
            case 10:  // 10 – Request Response message
            case 19:  // 19 – Heartbeat Subscription message
            case 20:  // 20 – Retransmission Request Message
            case 22:  // 22 – Refresh Request Message
            case 24:  // 24 – Heartbeat Response Message
            case 27:  // 27 – Extended Refresh Request
            case 34:  // 34 – Symbol Index Mapping Request
            case 35:  // 35 – Symbol Index Mapping Refresh Message
                continue;

            case 230:  // 230 – OpenBook Full Update Message
                break;

            case 231:  // 231 – OpenBook Delta Update Message
                break;

            default:
                // if(!message_type_field_[MsgType]) {
                //    std::cout << "\tUnexpected message type: "
                //              << MsgType
                //              << std::endl;
                //    message_type_field_[MsgType] = true;
                //}
                continue;
                // std::stringstream message_;
                // message_ << "Unexpected message type: " << MsgType;
                // throw std::logic_error(message_.str());
            }

            // +6
            // auto &SendTime = extract_e<std::uint32_t>(&buffer[6]);

            // +10
            std::string Symbol = std::string(&buffer[10], 11);
            boost::trim_right(Symbol);

            // +21
            // auto &MsgSize = extract_e<std::uint16_t>(&buffer[21]);

            // +23
            // If version before 1.2b (2017-12-14), this is a uint16_t,
            // afterwards it is uint32_t auto &SecurityIndex =
            // extract_e<std::uint32_t>(&buffer[23]);

            // +27
            auto &SourceTime = extract_e<std::uint32_t>(&buffer[27]);
            // auto source_time_hours   = std::floor(double(SourceTime) / (1000
            // * 60
            // * 60));

            // +31
            auto &SourceTimeMicroSecs = extract_e<std::uint16_t>(&buffer[31]);

            // +33
            // This field contains the current quote condition for the symbol.
            // The quote condition will be blank if no quote condition exists
            // (for example when the Book is fast). Liquidity Replenishment
            // Points (LRP)
            //
            //
            // Valid Values
            //  E  = Slow on the Bid due to LRP or GAP Quote
            //  F  = Slow on the Ask due to LRP or GAP Quote
            //  U  = Slow on the Bid and Ask due to LRP or GAP Quote
            //  W  = Slow Quote due to a Set Slow list on both the bid and offer
            //  sides
            // ' ' = No condition
            // auto QuoteCondition = buffer[33];

            // +34
            //
            // Trading Hours
            // PRE-OPENING SESSION: 3:30 A.M. ET
            // Limit orders can be entered and will be queued until the Limit
            // Order Auction at 4 a.m. ET OPENING SESSION: 4:00 A.M. TO 9:30
            // A.M. ET 3:59 a.m. to 4:00 a.m. ET - Opening Auction Freeze Period
            // 4:00 a.m. ET - Opening Auction 9:29 a.m. to 9:30 a.m. ET - Market
            // Order Freeze Period 9:30 a.m. ET - Market Order Auction Orders
            // that are eligible for the Opening Auction may not be cancelled
            // one minute prior to the Opening Session until the conclusion of
            // the Opening Auction.
            //
            // Market Orders and Auction-Only Limit Orders may not be canceled.
            // Market Orders and Auction-Only Limit. Orders may not be entered
            // on the same side as an imbalance.
            //
            // CORE TRADING SESSION: 9:30 A.M. TO 4:00 P.M. ET
            // 3:59 p.m. to 4:00 p.m. ET - Closing Auction Freeze Period
            // 4:00 p.m. ET - Closing Auction Run and Closing Price
            // Disseminated. All Day Orders entered for the core session will be
            // canceled. Market-on-Close (MOC) and Limit-on-Close (LOC) orders
            // cannot be canceled. MOC and LOC orders may not be entered on the
            // same side as an imbalance.
            //
            // EXTENDED HOURS: 4:00 PM TO 8:00 P.M. ET
            // 8:00 p.m. ET - Limit Orders entered after 4:00 p.m. ET are
            // canceled. 8:01 p.m. ET - Portfolio Crossing System (PCS) Cross
            // Orders are executed at the conclusion of extended trading hours.
            //
            // Valid Values:
            // P - Pre-Opening for messages sent before the stock is opened on a
            // trade or quote E – Early session B - Begin Accepting Orders O –
            // The stock has opened or re-opened | Opened (Core Session) L –
            // Late Session X - Closed C - The stock was closed from the Closing
            // template H - The stock is halted during a trading halt and has
            // not resumed
            auto TradingStatus = buffer[34];

            // +35
            // This field contains the sequence number assigned by the source
            // system to this message. The sequence number is unique only to a
            // given stock, hence orders for two different stocks may share the
            // same source sequence number.
            // auto &SourceSeqNum = extract_e<std::uint32_t>(&buffer[35]);

            // +39
            // This field contains the source-session identifier. This number is
            // incremented with every new source-session during the day. The
            // SymbolSeqNums may restart at a lower number with every new
            // session.
            // std::uint8_t SourceSessionID = buffer[39];

            // +40
            // \brief  The denominator code for the price fields in this
            //         message.Represents the number of digits after the decimal
            //         place in the price.
            //
            //         Examples:
            //           For a price of 12.1, the denomcode is 1.
            //           For a price of 13, the code is 0.
            std::uint8_t PriceScaleCode = buffer[40];

            // +41
            // This field contains the price (numerator) of this price point.
            // Note: The price is represented by the PriceScaleCode and the
            // PriceNumerator. For example, a price of 12.1 has a “price
            // numerator” of 12 and a scalecode of 1.
            auto &PriceNumerator = extract_e<std::uint32_t>(&buffer[41]);

            // +45
            // This field contains the total interest quantity at a price point.
            // auto &Volume         = extract_e<std::uint32_t>(&buffer[45]);

            // +49
            // The volume of the event taking place (that is, the size of the
            // order, cancel or execution).
            auto &ChgQty = extract_e<std::uint32_t>(&buffer[49]);

            // +53
            // This field contains the number of orders at the current price
            // point. auto &NumOrders = extract_e<std::uint16_t>(&buffer[53]);

            // +55
            // This field indicates the side of the order Buy/sell. Valid
            // Values: B – Buy S – Sell
            char Side = buffer[55];

            // +56
            // char Filler1 = buffer[56];

            // +57
            // The reason for this update
            // O = New order / additional interest added
            // C = Cancel
            // E = Execution
            // X = Multiple events
            // Space = This message is not an update
            char ReasonCode = buffer[57];

            // csv << Symbol << "," << TradingStatus << "," << ReasonCode
            //    << "," << SourceTime << "," << SourceTimeMicroSecs << "," <<
            //    PriceNumerator << ","
            //    << char('0' + PriceScaleCode) << "," << ChgQty << "," << Side
            //    << std::endl;

            time_point time_ = SourceTime * 1000 + SourceTimeMicroSecs;

            auto denom_ = std::pow<std::uint64_t>(10, PriceScaleCode);
            auto quote_ =
                quote(price(PriceNumerator, USD), denom_ / USD.denominator);

            callback(Symbol, TradingStatus, ReasonCode, time_, quote_, ChgQty,
                     (Side == 'B' ? limit_order::side_t::buy
                                  : limit_order::side_t::sell));

            // +58
            // char Filler2 = buffer[58];

            // +59
            // auto &LinkId1 = extract_e<std::uint32_t>(&buffer[59]);

            // +63
            // auto &LinkId2 = extract_e<std::uint32_t>(&buffer[63]);

            // +67
            // auto &LinkId3 = extract_e<std::uint32_t>(&buffer[67]);
        }
    }

    ///
    ///
    ///
    ///
    ///
    std::vector<std::tuple<std::string, char, char, time_point, quote,
                           std::uint32_t, limit_order::side_t>>
    parse_main_session(const std::string &filename,
                       const std::string &filter_symbol)
    {
        std::vector<std::tuple<std::string, char, char, time_point, quote,
                               std::uint32_t, limit_order::side_t>>
            main_session;
        main_session.reserve(size_t(1024));

        auto callback_ = [&](const std::string &symbol, char market_status,
                             char trade_status, time_point t,
                             const quote &limit, const std::uint32_t &quantity,
                             limit_order::side_t side) {
            if(market_status != 'O') {
                return;
            } else if(trade_status != 'O' && trade_status != 'C') {
                return;
            } else if(filter_symbol != symbol) {
                return;
            }
            main_session.push_back({symbol, market_status, trade_status, t,
                                    limit, quantity, side});
        };

        read_openbook(filename, callback_);
        return main_session;
    }

}  // namespace esl::data

#endif//ESL_OPENBOOK_HPP