
from esl.economics.iso4217 import ISO4217

class Price(object):
    __slots__ = ["value", "valuation"]
    value: int # can be negative as well
    valuation: ISO4217


