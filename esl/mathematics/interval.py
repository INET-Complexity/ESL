# \file   interval.hpp
#
# \brief
#
# \authors    Maarten P. Scholl
# \date       2018-01-31
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
from typing import List, TypeVar, Generic

N = TypeVar('N')


class Interval():  #Generic[N]):
    __slots__ = ["lower", "upper", "left_closed_", "right_closed_"]
    #lower: N
    #upper: N
    left_closed_: bool
    right_closed_: bool

    def __init__(self, lower, upper, left_closed_=True, right_closed_=True):
        self.lower = lower
        self.upper = upper
        self.left_closed_ = left_closed_
        self.right_closed_ = right_closed_
