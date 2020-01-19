from typing import NewType

from esl.mathematics.interval import Interval


TimePoint = NewType('TimePoint', int)


class TimeInterval(Interval):  #[TimePoint]):
    def __init__(self, lower: TimePoint, upper: TimePoint):
        super().__init__(lower, upper, True, False)
