# \file   parallel_environment.py
#
# \brief
#
# \authors    Maarten P. Scholl
# \date       2018-11-24
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
from typing import List, Dict

from mpi4py import MPI
import numpy


from esl.computation.environment import Environment
from esl.computation.timing import AgentTiming
from esl.simulation.model import Model
from esl.computation.distributed.protocol import Migration
from esl.simulation.identity import Identity
from esl.agent import Agent
from esl.computation.distributed.protocol import Activation, NodeIdentifier


class MpiEnvironment(Environment):

    agent_locations_: Dict[Identifier, NodeIdentifier] # Identifier[Agent]

    def __init__(self):
        super().__init__()
        self.communicator = MPI.COMM_WORLD

    def is_coordinator(self) -> bool:
        return 0 == self.communicator.Get_rank()

    def migrate(self, simulation: Model, timing: AgentTiming) -> None:
        pass

    def process_migrations(self, simulation: Model) -> None:
        pass

    def migrate_agents(self) -> List[Migration]:
        return []

    def activate(self) -> int:
        activated_locally_ = numpy.empty(self.communicator.Get_size(), dtype=list)
        for i in range(self.communicator.Get_size()):
            activated_locally_[i] = [Activation(self.rank, j) for j in self.activated_]
        activations_stacked_ = numpy.empty(self.communicator.Get_size(), dtype=list)
        self.communicator.Alltoall(activated_locally_, activations_stacked_)

        activations_: List[Activation] = []
        result_ = 0
        for s in activations_stacked_:
            for activation in s:
                agent_locations_[activation.activated.identifier] = activation.location
                result_ += 1
        return result_

    def deactivate(self) -> int:
        pass

    def before_step(self) -> None:
        pass

    def after_step(self, simulation: Model) -> None:
        pass

    def activate_agent(self, a: Identity[Agent]) -> None:
        pass

    def deactivate_agent(self, a: Identity[Agent]) -> None:
        pass

    def send_messages(self, simulation: Model) -> int:
        pass
