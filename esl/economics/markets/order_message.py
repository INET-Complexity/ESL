from esl.agent import Agent
from esl.interaction.header import MessageCode
from esl.interaction.message import Message
from esl.simulation.identity import Identity
from esl.simulation.time import TimePoint


class OrderMessage(Message):

    def __init__(self, sender: Identity[Agent], recipient: Identity[Agent], sent: TimePoint = TimePoint(),
                 received: TimePoint = TimePoint(), type: MessageCode = 0):
        super().__init__(sender, recipient, sent, received, type)