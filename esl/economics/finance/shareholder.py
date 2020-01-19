from typing import Dict, Set, Tuple

from esl.simulation.identity import Identity
from esl.economics.finance.company import Company
from esl.economics.finance.share import Share

class Shareholder(object
                  #, Owner[Cash]
                  #, Owner[Stock]
                  #, IdentifiableAs[Shareholder]
                  ):

    stocks: Dict[Tuple[Identity[Company], Share], Identity[Property]]

    prices: Dict[Identity[Property], Price] # used for valuation on balance sheet, if if agent is not a trader

    shares: Dict[Share, int]

    ex_dividend_dates: Set[Tuple[TimePoint, Identity[Company]]]
