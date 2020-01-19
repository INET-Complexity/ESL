from typing import List

from esl.law.property import Property
from esl.economics.fungibility import Infungible
from esl.economics.tangibility import Intangible
from esl.simulation.identity import Identity
from esl.agent import Agent


class Contract(Property, Infungible, Intangible):
    parties: List[Identity[Agent]]

    def __init__(self, parties=None):
        super().__init__()
        if parties is None:
            parties = list()
        self.parties = parties
