from esl.computation.environment import Environment
from esl.simulation.parameter.parametrization import Parametrization
from esl.simulation.time import TimePoint, TimeInterval
from esl.simulation.world import World
from esl.simulation.agent_collection import AgentCollection


class Model(object):
    environment: Environment
    parameters: Parametrization
    start: TimePoint
    end: TimePoint
    sample: int
    world: World
    agents: AgentCollection

    def __init__( self
                , environment: Environment = Environment()
                , parameters: Parametrization = Parametrization()
                , agents: AgentCollection = AgentCollection()
                ):
        self.environment = environment
        self.parameters = parameters
        self.start = parameters['start']
        self.end = parameters['end']
        self.sample = parameters['sample']
        self.world = World()
        self.agents = agents

    def initialize(self):
        pass

    def step(self, step: TimeInterval) -> TimeInterval:
        #if step is None:
        #    return TimeInterval(self.time, self.end)
        self.environment.before_step(self.agents)
        first_event_ = self.start
        round_ = 0
        while step.upper > first_event_:
            first_event_ = step.upper
            for i, a in self.agents.local_agents_.items():
                seed_ = (hash(i), step.lower, round_, self.sample)
                try:
                    first_event_ = min(first_event_,a.process_messages(step, seed_))
                    first_event_ = min(first_event_,a.act(step, seed_))
                except KeyboardInterrupt as ki:
                    # stop running, but exit normally
                    pass
                except BaseException as e:
                    pass
                a.inbox.clear()
            self.environment.send_messages(self.agents)
        self.environment.after_step(self.agents)
        return first_event_

    def run(self):
        self.initialize()
        step_ = TimeInterval(self.start, self.end)
        while step_.lower < self.end:
            changes_ = 0
            changes_ += self.activate()
            changes_ += self.deactivate()

            step_.lower = self.step(step_)
        self.terminate()

    def terminate(self):
        pass

    def __repr__(self):
        return f"Model({self.parameters}, {self.agents})"
