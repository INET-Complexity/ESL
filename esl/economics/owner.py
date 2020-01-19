from typing import Dict, TypeVar, Generic

from esl.agent import Agent
from esl.economics.accounting.inventory import Inventory

A = TypeVar('A')


class Owner( Generic[A]
           , Agent
           # , IdentifiableAs[Owner[A]]
           ):
    inventory: Inventory

    def __init__(self):
        super().__init__()
