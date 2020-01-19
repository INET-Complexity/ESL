from typing import List

from esl.economics.markets.quote import Quote


class DemandSupplyFunction(object):
    def excess_demand(self, quotes: List[Quote], variables: List[float]) -> List[float]:
        pass