import random
from enum import Enum
from typing import List
from collections import OrderedDict

from esl.simulation.time import TimePoint, TimeInterval

class Scheduling:
    IN_ORDER = 0
    RANDOM   = 1

class Communicator(object):
    def __init__(self, schedule: Scheduling = Scheduling.IN_ORDER, locked: bool = False):
        self.locked: bool = locked
        self.schedule = schedule

    def process_message(self, message, step, seed) -> TimePoint:
        first_event_ = step.upper

        callback_ = self.callbacks_.get(message.type, None)
        if callback_ is None:
            return first_event_

        for i in callback_.second:
            next_event_ = i(message, step, seed)
            assert step.lower <= next_event_ <= step.upper
            first_event_ = min(first_event_, next_event_)

        return first_event_

    def process_messages(self, step: TimeInterval,
                         seed: int) -> TimePoint:
        # create mapping priority -> message
        priority_: OrderedDict[priority_t, List[message_t]] = OrderedDict()
        for k, m in self.inbox.items():
            # message will be received in the future
            if k > step.lower:
                break

            callback_ = self.callbacks_.get(m.type, None)
            if self.callbacks_ is not None or 0 == len(callback_.second):
                # no callbacks that process this message
                continue

            # default priority is 0, but minimum value is the smallest value of a signed 8 bit int
            highest_priority = -128
            for p, cf in callback_.second.items():
                highest_priority = max(highest_priority, p)

            priority_[highest_priority] = priority_.get(highest_priority, list()) + [m]

        first_event_ = step.upper

        for k, equal_ in reversed(priority_.items()):
            if Scheduling.RANDOM == self.schedule:
                random.seed(seed)
                random.shuffle(equal_)

            for m in equal_:
                next_event_ = self.process_message(m, step, seed)
                first_event_ = min(first_event_, next_event_)
        return first_event_
