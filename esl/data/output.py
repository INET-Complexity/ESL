from typing import List, Tuple, Any

from esl.data.output_base import OutputBase


class Output(OutputBase):
    values: List[Tuple[int, Any]]

    def __init__(self, name: str, values: List[Tuple[int, Any]] = []):
        OutputBase.__init__(self, name=name)
        self.values = values

