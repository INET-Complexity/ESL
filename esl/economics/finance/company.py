# \file   company.py
#
# \brief
#
# \authors    Maarten P. Scholl
# \date       2018-04-27
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
from typing import Dict

from esl.simulation.identity import Identity
from esl.law.organization import Organization
from esl.economics.finance.share import Share
from esl.economics.finance.shareholder import Shareholder


class Company(Organization, Shareholder):  # , IdentifiableA[Company]):
    shares_outstanding: Dict[Share, int]
    shareholders: Dict[Identity[Shareholder], Dict[Share, int]]

    def __init__(self, shares_outstanding=None, shareholders=None):
        super().__init__()
        if shares_outstanding is None:
            shares_outstanding = dict()
        if shareholders is None:
            shareholders = dict()
        self.shares_outstanding = shares_outstanding
        self.shareholders = shareholders
