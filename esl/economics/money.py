from esl.economics.asset import Asset
from esl.economics.fungibility import Fungible
from esl.economics.iso4217 import ISO4217


class Money(Asset, Fungible):
    denomination: ISO4217

    def __init__(self, denomination: ISO4217):
        self.denomination = denomination
