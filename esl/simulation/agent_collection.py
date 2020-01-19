from typing import Dict, Set, List

from esl.agent import Agent
from esl.simulation.identity import Identity


class AgentCollection(object):
    local_agents_: Dict[Identity, Agent]# Dict[Identity[Agent], Agent]
    global_agents_: Set[Identity]  # Identity[Agent]
    activated_: List[Identity] # Identity[Agent]
    deactivated_: List[Identity] # Identity[Agent]

    def __init__( self
                , local_agents: Dict[Identity, Agent] = None
                , global_agents: Set[Identity] = None
                ):
        if local_agents is None:
            local_agents = dict()
        if global_agents is None:
            global_agents = set()

        self.local_agents_ = local_agents
        self.global_agents_ = global_agents

    def activate(self, a: Agent):
        self.global_agents_.add(a.identifier)
        self.local_agents_[a.identifier] = a
        self.activated_.append(a)  # self.environment_.activate_agent(a)

    def deactivate(self, a: Agent):
        self.global_agents_.remove(a.identifier)
        del self.local_agents_[a.identifier]
        self.deactivated_.append(a)  # self.environment_.deactivate_agent(a)


    def __repr__(self):
        return f"AgentCollection(local_agents={self.local_agents_},global_agents={self.global_agents_})"
