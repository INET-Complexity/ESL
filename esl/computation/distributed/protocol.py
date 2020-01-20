# \file   protocol.py
#
# \brief
#
# \authors    Maarten P. Scholl
# \date       2018-09-05
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
from typing import NewType


from esl.agent import Agent
from esl.simulation.identity import Identity


NodeIdentifier = NewType('Digit', int)


class Activation(object):
    __slots__ = ['location', 'activated']
    location: NodeIdentifier
    activated: Identity  # Identity[Agent]

    def __init__(self
                 , location: int
                 , activated: Identity  # Identity[Agent]
                 ):
        self.location = location
        self.activated = activated


class Deactivation(object):
    __slots__ = ['deactivated']
    deactivated: Identity  # Identity[Agent]

    def __init__(self, deactivated: Identity[Agent]):
        self.deactivated = deactivated


class Migration(object):
    __slots__ = ['source', 'target', 'migrant']
    source: NodeIdentifier
    target: NodeIdentifier
    migrant: Identity  # Identity[Agent]

    def __init__(self
                 , source: NodeIdentifier
                 , target: NodeIdentifier
                 , migrant: Identity  # Identity[Agent]
                 ):
        self.source = source
        self.target = target
        self.migrant = migrant
