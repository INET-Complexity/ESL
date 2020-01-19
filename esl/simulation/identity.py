from typing import List, TypeVar, Generic, NewType

identifiable_type_ = TypeVar('identifiable_type_')


class Identity():  # Generic[identifiable_type_]):
    __slots__ = ['digits']

    Digit = NewType('Digit', int)

    def __init__(self, digits=None):
        if digits is None:
            digits = []
        self.digits = digits

    def __eq__(self, other):
        return self.digits.__eq__(other.digits)

    def __ne__(self, other):
        return self.digits.__ne__(other.digits)

    def __lt__(self, other):
        return self.digits.__lt__(other.digits)

    def __le__(self, other):
        return self.digits.__le__(other.digits)

    def __gt__(self, other):
        return self.digits.__gt__(other.digits)

    def __ge__(self, other):
        return self.digits.__ge__(other.digits)

    def __hash__(self):
        seed = 0
        for d in self.digits:
            # taken from boost::hash_combine
            seed ^= d + 0x9e3779b9 + (seed << 6) + (seed >> 2)
        return seed

    def __repr__(self):
        return "-".join(map(str, self.digits))

    def __str__(self):
        return self.__repr__()
