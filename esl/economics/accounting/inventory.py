from typing import Dict

from esl.quantity import Quantity
from esl.economics.accounting.standard import Standard
from esl.economics.price import Price
from esl.law.property import Property


class InsufficientInventory(Exception):
    before: Quantity
    withdrawal: Quantity

    def __init__(self, before, withdrawal):
        self.before = before
        self.withdrawal = withdrawal
        assert withdrawal.amount > 0


class Inventory(object):
    items: Dict[Property, Quantity]

    def __init__(self, items: Dict[Property, Quantity] = dict()):
        self.items = items

    def insert(self, p: Property):
        pass

    def insert(self, i: Inventory):
        pass

    def erase(self, p: Property):
        pass

    def erase(self, i: Inventory):
        pass

    def value(self, a: Standard) -> Price:
        pass



