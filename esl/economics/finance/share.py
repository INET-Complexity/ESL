

class Share(object):
    __slots__ = ["rank", "votes", "dividend", "preference", "cumulative", "redeemable"]

    def __init__(self
                , rank: int = 0
                , votes: int = 1
                , dividend: bool = True
                , preference: float = 0.0
                , cumulative: bool = False
                , redeemable: bool = False):
        self.rank = rank
        self.votes = votes
        self.dividend = dividend
        self.preference = preference
        self.cumulative = cumulative
        self.redeemable = redeemable