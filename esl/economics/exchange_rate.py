from esl.economics.rate import Rate

class ExchangeRate(Rate): # unsigned int
    def __init__(self, numerator: int, denominator: int) -> None:
        assert numerator >= 0
        assert denominator > 0
        super().__init__(numerator, denominator)