from esl.economics.asset import Asset
from esl.economics.fungibility import Fungible
from esl.economics.tangibility import Tangible


class Commodity(Asset, Fungible, Tangible):
    pass
