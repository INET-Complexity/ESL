from typing import Union

from esl.economics.price import Price
from esl.economics.exchange_rate import ExchangeRate

class Quote(object):
    type: Union[ExchangeRate, Price]

    def __init__(self, type: Union[ExchangeRate, Price]):
        self.type = type

