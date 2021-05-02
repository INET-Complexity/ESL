# \file   walras.py
#
# \brief
#
# \authors    Maarten P. Scholl
# \date       2021-01-25
# \copyright  Copyright 2017-2021 The Institute for New Economic Thinking,
#             Oxford Martin School, University of Oxford
#
#             Licensed under the Apache License, Version 2.0 (the "License");
#             you may not use this file except in compliance with the License.
#             You may obtain a copy of the License at
#
#                 http://www.apache.org/licenses/LICENSE-2.0
#
#             Unless required by applicable law or agreed to in writing,
#             software distributed under the License is distributed on an "AS
#             IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
#             express or implied. See the License for the specific language
#             governing permissions and limitations under the License.
#
#             You may obtain instructions to fulfill the attribution
#             requirements in CITATION.cff
#
import esl

print(esl.version())

# The market trades one (not further defined) propert for US Dollars
from esl.law import property, property_identity

# import the price class and the USD predefined currency
from esl.economics import price, currencies

USD = currencies.USD

# import the quote class that defines an economic exchange
from esl.economics.markets import quote

# import the excess demand model
from esl.economics.markets.walras import excess_demand_model, differentiable_order_message


# create an identifier for the economic property being traded in the market,
# so that we can tell two or more properties apart
i  = property_identity([1])
i2 = property_identity([2])
# create an abstract property with this identifier. Its details do not matter
p  = property(i)
p2 = property(i2)

# note that by default, we use fixed precisions for prices! This is $1.23:
initial_price = price(123, USD)
# if we are not afraid of floating point truncation errors, we could also write:
initial_price = price.approximate(1.23, USD)
# or if we want to do a conversion ourselves:
initial_price = price(int(1.23 * USD.denominator), USD)
print(initial_price)

# We are now going to construct a `quote`, which describes how exchanges in the market happen.
# The quote object will tell our market
#   - 'type': what the prices are: are they monetary values, raw exchange rates like on a barter market, or interest rates or something else?
#   - 'lot size': the quantity for which the prices are defined (this allows for prices with the correct precision, preventing rounding errors)
# the lot size is 1 by default, so this quote means "buy 1 property in exchange for $1.23"
initial_quote = quote(initial_price)

print(f"The initial quote is {initial_quote}") # prints 1@USD(1.23)

# we now create the Walrasian market model, and give it a dictionary:
# the property identifiers are keys,  and and initial quotes are the values
model = excess_demand_model({p: initial_quote, p2: initial_quote})


# In the absence of a fully defined agent based model, we need to cut some corners here to demonstrate the market
# mechanism:

# Here we create a dummy market agent, and a dummy trader agent,
# because we need at least two agent identifiers for accounting purposes
market_agent = esl.simulation.identity([0])
trader_agent = esl.simulation.identity([1])

# Here we define an order message. In ESL, all messages are defined as a class
class my_order(differentiable_order_message):
    def __init__(self, sender, recipient, sent, received):
        super().__init__(sender, recipient, sent, received)
        # TODO (Maarten): this should be exposed through base class
        self.sender = sender

    def excess_demand(self, quotes):
        """
            This is our excesss demand function. It receives a dict with:
            - keys being the property identifiers
            - values being a tuple with the previous quote, and a real number variable such that
                float(quote) * variable
                is a real number that we can use to give demand in real numbers
                (in essence, a continuous function rather than a function with discrete steps)

            In this example, the demand function is simply $3.00 - p(t) for the first property, $4.00 - p(t) for the second,
            and so on.

        :param quotes: A dict with property_identifier keys and pairs (quote, variable)
        :return:
        """
        print(f"The Walrasian price setter suggests the following prices: {quotes}")
        ed = {k: ((i+3.) - (float(v[0]) * v[1])) for i,  (k, v) in enumerate(quotes.items())}
        print(f"Agent {self.sender}'s excess demand at these prices is: {ed}")
        return ed


# normally the simulation::model class would determine the time, but this is a one-period example
current_time = 0

# The order is sent and received immediately
message = my_order(trader_agent, market_agent, current_time, current_time)

# give the message to the model. Normally, we'd use a market agent (esl.economics.markets.walras.price_setter) to do this
model.excess_demand_functions = [message]

# compute solution and print the result
multipliers = model.compute_clearing_quotes()

# multipliers
# print(multipliers)

# Since the result is a floating number multiplier to the initial price, we need to decide carefully
# how to round the result back to dollars and cents
for k, v in multipliers.items():

    new_price = price(round(float(initial_price) * v * USD.denominator), USD)
    print(f"New price for property {k}: {new_price}")
