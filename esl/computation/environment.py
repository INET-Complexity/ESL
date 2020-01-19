# \file   environment.py
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
from esl.simulation.agent_collection import AgentCollection


class Environment(object):
    def __init__(self):
        pass

    ___ = """
    def run(self, simulation: Model) -> None:


        simulation.initialize()
        step_ = (simulation.start, simulation.end)
        while step_.lower < simulation.end:
            changes_ = 0
            changes_ += self.activate()
            changes_ += self.deactivate()

            step_.lower = simulation.step(step_)

        simulation.terminate()
        """

    def activate(self, agents: AgentCollection) -> int:
        pass

    def deactivate(self, agents: AgentCollection) -> int:
        pass

    def before_step(self, agents: AgentCollection) -> None:
        pass

    def after_step(self, agents: AgentCollection) -> None:
        pass

    def send_messages(self, agents: AgentCollection) -> int:
        """
        Send and receive messages, to all nodes all at the same time
        :param agents:
        :return:
        """
        messages_ = 0
        for i, a in agents.local_agents_:
            for m in a.outbox:
                if m.recipient not in agents.local_agents_:
                    raise Exception(f"agent not found {m.recipient.__repr__()}")

                agents.local_agents_[m.recipient].inbox.insert(  (m.received, m)  )
                messages_ += 1
            a.outbox.clear()
        return messages_
