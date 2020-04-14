/// \file   book.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-04-05
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


#include <esl/economics/markets/order_book/book.hpp>
#include <esl/data/log.hpp>


namespace esl::economics::markets::order_book {

    bool book::insert(const order &order)
    {
        if(order.getSide() == order::buy)
            orders_bid.insert(bid_t::value_type(order.getPrice(), order));
        else
            orders_ask.insert(ask_t::value_type(order.getPrice(), order));
        return true;
    }

    void book::erase(const order &order)
    {
        std::string id = order.getClientID();
        if(order.getSide() == order::buy) {
            bid_t::iterator i;
            for(i = orders_bid.begin(); i != orders_bid.end(); ++i)
                if(i->second.getClientID() == id) {
                    orders_bid.erase(i);
                    return;
                }
        } else if(order.getSide() == order::sell) {
            ask_t::iterator i;
            for(i = orders_ask.begin(); i != orders_ask.end(); ++i)
                if(i->second.getClientID() == id) {
                    orders_ask.erase(i);
                    return;
                }
        }
    }

    bool book::match(std::queue<order> &orders)
    {
        while(true) {
            if(!orders_bid.size() || !orders_ask.size())
                return orders.size() != 0;

            bid_t::iterator iBid = orders_bid.begin();
            ask_t::iterator iAsk = orders_ask.begin();


            if(iBid->second.getPrice() >= iAsk->second.getPrice()) {
                order &bid = iBid->second;
                order &ask = iAsk->second;

                match(bid, ask);
                orders.push(bid);
                orders.push(ask);

                if(bid.isClosed())
                    orders_bid.erase(iBid);
                if(ask.isClosed())
                    orders_ask.erase(iAsk);
            } else
                return orders.size() != 0;
        }
    }

    order &book::find(order::side side, std::string id)
    {
        if(side == order::buy) {
            bid_t::iterator i;
            for(i = orders_bid.begin(); i != orders_bid.end(); ++i)
                if(i->second.getClientID() == id)
                    return i->second;
        } else if(side == order::sell) {
            ask_t::iterator i;
            for(i = orders_ask.begin(); i != orders_ask.end(); ++i)
                if(i->second.getClientID() == id)
                    return i->second;
        }
        throw std::exception();
    }

    void book::match(order &bid, order &ask)
    {
        auto price = ask.getPrice();
        int quantity = 0;

        if(bid.getOpenQuantity() > ask.getOpenQuantity())
            quantity = ask.getOpenQuantity();
        else
            quantity = bid.getOpenQuantity();

        bid.execute(price, quantity);
        ask.execute(price, quantity);
    }

    void book::display() const
    {
        bid_t::const_iterator iBid;
        ask_t::const_iterator iAsk;


        LOG(trace) << "BIDS:" << std::endl;

        LOG(trace) << "-----" << std::endl << std::endl;
        for(iBid = orders_bid.begin(); iBid != orders_bid.end(); ++iBid)
            LOG(trace) << iBid->second << std::endl;

        LOG(trace) << std::endl << std::endl;

        LOG(trace) << "ASKS:" << std::endl;
        LOG(trace) << "-----" << std::endl << std::endl;
        for(iAsk = orders_ask.begin(); iAsk != orders_ask.end(); ++iAsk)
            LOG(trace) << iAsk->second << std::endl;
    }
}
