# \file   header.py
#
# \brief
#
# \authors    Maarten P. Scholl
# \date       2019-04-02
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
from esl.simulation.identity import Identity
from esl.simulation.time import TimePoint
from esl.agent import Agent


MessageCode = int

MessageMask: MessageCode = 0x8000_0000_0000_0000


class Header(object):
    __slots__ = ["type", "sender", "recipient", "sent", "received"]
    type: MessageCode
    sender: Identity[Agent]
    recipient: Identity[Agent]
    sent: TimePoint
    received: TimePoint

    def __init__( self
                , sender: Identity[Agent]
                , recipient: Identity[Agent]
                , sent: TimePoint = TimePoint()
                , received: TimePoint = TimePoint()
                , type: MessageCode = 0):
        self.type = type
        self.sender = sender
        self.recipient = recipient
        self.sent = sent
        self.received = received


