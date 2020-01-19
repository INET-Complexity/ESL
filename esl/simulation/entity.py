from typing import List, TypeVar, Generic

from esl.simulation.identity import Identity


class Entity():#Generic[E]):
    __slots__ = ["identifier", "children"]
    identifier: Identity#[E]
    children: Identity.Digit#[E].Digit

    def __init__(self, identifier: Identity = Identity()):
        self.identifier = identifier
        self.children = 0

    def __eq__(self, other: 'Entity'):
        return self.identifier.__eq__(other.identifier)

    def __ne__(self, other: 'Entity'):
        return self.identifier.__ne__(other.identifier)

    def __hash__(self):
        return self.identifier.__hash__()

    def __repr__(self):
        return f"Entity[E] {self.identifier}"

    def __str__(self):
        return self.__repr__()