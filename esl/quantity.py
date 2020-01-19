# \file   quantity.py
#
# \brief  A class used to enforce conversion without numerical error when
#         counting property, goods and other items.
#
# \authors    Maarten P. Scholl
# \date       2018-02-05
# \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
#             Oxford Martin School, University of Oxford
#
#             Licensed under the Apache License, Version 2.0 (the "License");
#             you may not use this file except in compliance with the License.
#             You may obtain a copy of the License at
#
#                 http://www.apache.org/licenses/LICENSE-2.0
#
#             Unless required by applicable law or agreed to in writing,
#             software distributed under the License is distributed on an "AS
#             IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
#             express or implied. See the License for the specific language
#             governing permissions and limitations under the License.
#
#             You may obtain instructions to fulfill the attribution
#             requirements in CITATION.cff
#
from typing import overload, List


class Quantity(object):
    __slots__ = ['amount', 'basis']
    amount: int
    basis: int

    def __init__(self, amount: int, basis: int):
        assert amount >= 0
        assert basis >= 1

        self.amount = amount
        self.basis = basis

    @overload
    def __add__(self, operand: int) -> "Quantity":
        return Quantity(self.amount + (operand * self.basis), self.basis)

    @overload
    def __add__(self, operand: "Quantity") -> "Quantity":

        numerator_ = self.amount * operand.basis + operand.amount * self.basis
        remainder_ = numerator_ % operand.basis

        if 0 == remainder_:
            return Quantity(numerator_ // operand.basis, self.basis)

        # if(EXCEPTION_ON_BASIS_CHANGE) {
        #    throw std::logic_error("EXCEPTION_ON_BASIS_CHANGE")

        coremainder_ = numerator_ % self.basis

        if 0 == coremainder_:
            return Quantity(numerator_ // self.basis, operand.basis)

        return Quantity(numerator_, self.basis * operand.basis)

    def __add__(self, operand) -> "Quantity":
        raise NotImplementedError()

    @overload
    def __sub__(self, operand: int):
        pass

    @overload
    def __sub__(self, operand: "Quantity"):
        pass

    def __sub__(self, operand) -> "Quantity":
        raise NotImplementedError()

    @overload
    def __mul__(self, operand: int):
        pass

    @overload
    def __mul__(self, operand: "Quantity"):
        pass

    def __mul__(self, operand):
        raise NotImplementedError()

    @overload
    def __truediv__(self, operand: int) -> List[int]:
        pass

    @overload
    def __truediv__(self, operand: "Quantity") -> List[int]:
        pass

    def __truediv__(self, operand):
        raise NotImplementedError()

    @overload
    def __floordiv__(self, operand: int) -> int:
        pass

    @overload
    def __floordiv__(self, operand: "Quantity") -> int:
        pass

    def __floordiv__(self, operand):
        raise NotImplementedError()