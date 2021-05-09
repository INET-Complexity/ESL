import esl

print(esl.version())

from esl.economics import *
from esl.simulation import *
from esl.economics.markets import *
from esl.economics.markets.order_book import *

import numpy as np


USD = currencies.USD

# trades in this market are in multiples of the lot size
# e.g. a lot_size of 100 means that a buy order for a quantity of 3 buys 300 shares
# A larger lot size enables small tick sizes
lot_size = 1

lower_price = price(    0, USD)
upper_price = price(20_00, USD)

# we will create a simple equities central order book
ob = static_order_book( quote(lower_price)
                      , quote(upper_price)
                      , 100000)



# helper function to create limit orders submitted to the order book.
# note that ESL uses fixed-precision for prices internally, and that the use of
# a floating point type here relies on the developer understanding the
# conversion.
def create(trader_id: identity, limit: price, size: int, side:side_t  = side_t.sell):
    ticker_dummy_ = ticker(identity([1]), identity([2]))

    q = quote(limit)
    q.lot = lot_size #* USD.denominator

    lifetime = lifetime_t.good_until_cancelled

    return limit_order_message(ticker_dummy_, trader_id, side , q, size , lifetime)


trader_a = identity([0,1])
trader_b = identity([0,2])



ob.insert(create(trader_b, price(4_99, USD), 750, side_t.sell,  ))
ob.insert(create(trader_b, price(4_75, USD), 500, side_t.sell,  ))
ob.insert(create(trader_a, price(4_74, USD), 250, side_t.sell,  ))
ob.insert(create(trader_b, price(4_74, USD), 100, side_t.buy,  ))

for r in ob.reports:
    print(r.state, r.limit, r.owner)



