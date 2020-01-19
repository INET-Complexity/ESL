from typing import Any, Dict

from esl.data.output import Output

class Producer(object):
    outputs: Dict[str, Output]

    def __init__(self, outputs: Dict[str, Any] = dict()):
        pass

    def create_output(self, name: str):
        self.outputs[name] = Output()
