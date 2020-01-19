
from esl.geography.iso31661alpha2 import ISO31661Alpha2
from esl.economics.iso4217 import ISO4217


class Jurisdiction(object):
    sovereign: ISO31661Alpha2
    tender: ISO4217

    def __init__(self, sovereign: ISO31661Alpha2, tender: ISO4217):
        self.sovereign = sovereign
        self.tender = tender
