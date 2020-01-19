from typing import Union

from esl.agent import Agent
from esl.law.owner import Owner
from esl.law.property import Property
from esl.law.jurisdiction import Jurisdiction


class LegalPerson(Agent, Owner[Property]):
    #representation: Union[LegalEntity, NaturalPerson, Government]
    primary_jurisdiction: Jurisdiction

    def __init__( self
                , representation#: Union[LegalEntity, NaturalPerson, Government]
                , primary_jurisdiction: Jurisdiction):
        self.representation = representation
        self.primary_jurisdiction = primary_jurisdiction
