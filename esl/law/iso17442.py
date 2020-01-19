

class ISO17442(object):
    local: str
    code: str

    def __init__(self, local: str, code: str):
        assert len(local) == 4
        assert len(code) == 12
        self.local = local
        self.code = code
