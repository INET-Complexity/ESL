from typing import Dict, Generic

from esl.simulation.time import TimePoint


class ParameterBase(object):
    pass


class Constant(ParameterBase): # Generic[V]
    __slots__ = ["value"]

    def __init__(self, value):
        self.value = value

    def __repr__(self):
        return self.value.__repr__()


class Parametrization(object):
    __slots__ = ["values"]
    values: Dict[str, ParameterBase]

    def __init__( self
                , start: TimePoint = 0
                , end: TimePoint = None
                , sample: int = 0):
        self.values = { "start": Constant(start)
                      , "end": Constant(end)
                      , "sample": Constant(sample)
                      }

    def __repr__(self):
        return self.values.__repr__()

    def __getitem__(self, item):
        return self.values.__getitem__(item)

    def __setitem__(self, key, value):
        return self.values.__setitem__(key, value)