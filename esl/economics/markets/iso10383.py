

class ISO10383(object):
    __slots__ = ["code"]
    code:str

    def __init__(self, code:str) -> None:
        assert len(code) == 4
        for i in range(4):
            assert (ord('A') <= ord(code[i]) <= ord('Z')) or (ord('0') <= ord(code[i]) <= ord('9'))
        super().__init__()
        self.code = code

