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
from typing import List

import mpi4py

from esl.computation.environment import Environment
from esl.computation.timing import AgentTiming
from esl.simulation.model import Model
from esl.computation.distributed.protocol import Migration
from esl.simulation.identity import Identity
from esl.agent import Agent


class MpiEnvironment(Environment):

    def __init__(self):
        super().__init__()

    def is_coordinator(self) -> bool:
        pass

    def migrate(self, simulation: Model, timing: AgentTiming) -> None:
        pass

    def process_migrations(self, simulation: Model) -> None:
        pass

    def migrate_agents(self) -> List[Migration]:
        pass

    def activate(self) -> int:
        pass

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
