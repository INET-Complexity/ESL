
from esl.geography.iso31661alpha2 import ISO31661Alpha2

class ISIN(object):
    issuer: ISO31661Alpha2

    def __init__(self, issuer: ISO31661Alpha2, code = "000000000"):
        self.issuer = issuer
        self.code = code

    