
class ISO4217(object):

    def __init__(self, code :str = "XXX", denominator :int = 100):
        assert len(code) == 3
        for i in range(3):
            assert ord('A') <= ord(code[i]) <= ord('Z')
        assert denominator >= 1

        self.code = code
        self.denominator = denominator

    def __hash__(self) -> int:
        return (           (ord(self.code[0]) - ord('A'))
                    + 26 * (ord(self.code[1]) - ord('A'))
               + 26 * 26 * (ord(self.code[2]) - ord('A')))

    def __str__(self):
        return self.code
