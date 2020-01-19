from typing import Dict, Set

from esl.simulation.identity import Identity
from esl.economics.markets.quote import Quote
from esl.law.property import Property
from esl.law.owner import Owner
from esl.agent import Agent


class Market(Agent
            #, IdentifiableAs[Market]
             ):
    traded_properties: Dict[Identity[Property], Quote]
    participants: Set[Identity[Owner[Property]]]

    def __init__(self, traded_properties: Dict[Identity[Property], Quote]):
        super().__init__()
        self.traded_properties = traded_properties


