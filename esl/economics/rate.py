from esl.mathematics.rational import Rational


class Rate(Rational):
    def __init__(self, numerator: int, denominator: int) -> None:
        super().__init__()
        self._numerator = numerator
        self._denominator = denominator

