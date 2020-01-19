if __name__ == "__main__" and __package__ is None:
    from sys import path
    from os.path import dirname as dir

    path.append(dir(path[0]))
    __package__ = "agent"

____ = """
from esl.simulation import entity
from esl.interaction.communicator import Communicator
from esl.data.producer import Producer


class Agent(Entity['Agent'], Communicator, Producer):
    __slots__ = []

    def __init__(self):
        super().__init__()

    def __hash__(self):
        return hash(self.identifier)

"""

class Agent:
    pass