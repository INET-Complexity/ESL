import esl
from esl import agent
from esl.economics import *

USD = currencies.USD


import numpy as np


class TradingAgent(esl.agent):
    def __init__(self, identifier, weights, sigmae, kmax, Lmin, Lmax, fundamental, tau, wakeup_times):
        super().__init__(identifier)

        sigmaF, sigmaM, sigmaN = weights

        self.weight_fundamental = np.abs(sigmaF * np.random.randn())
        self.weight_technical = np.random.normal(0, sigmaM)
        self.weight_noise = np.random.normal(0., sigmaN)

        self.horizon = np.random.randint(Lmin, Lmax)

        self.spread = np.random.normal(0., kmax)
        self.fcast = 0.
        self.pfcast = 0.

        self.order_schedule = []

        self.tau = tau


        self.forecast_norm = 1. / (self.weight_fundamental + self.weight_technical + self.weight_noise)
        self.wakeup_times = wakeup_times

        self.Lmax = Lmax
        self.fundamental = fundamental
        self.sigmae = sigmae

        self.fundamental = 0.
        self.noise = 0.
        self.chartist = np.zeros(Lmax)
        self.v = 0.


    def update_forecasts(self, t, pric, ret):
        self.fundamental = np.log(float(self.fundamental) / float(pric))
        self.noise = self.sigmae * np.random.randn()
        self.v = np.var(ret[0:t])
        revrets = ret[t:(t - self.Lmax):-1]
        self.chartist = np.cumsum(revrets) / np.arange(1., float(min(len(revrets), self.Lmax + 1)))

    def act( self
           , step: esl.simulation.time_interval
           , seed: esl.agent
           ) -> esl.simulation.time_point:

        print(self.inbox)

        t = step.lower
        p = price.approximate(1.23, USD)  # float(prices[t])
        ret = [1.]

        self.update_forecasts(t, p, ret)

        self.update_beliefs(p, self.tau)

        if self.pfcast > p:
            self.order_schedule = [price.approximate((1. - self.spread) * float(self.pfcast), p.valuation)]
        else:
            self.order_schedule = [price.approximate((1. + self.spread) * float(self.pfcast), p.valuation)]

        next_event = min(filter(lambda w: w > step.lower, self.wakeup_times), default=step.upper)

        #
        #
        # def create(trader_id: identity, limit: price, size: int, side: side_t = side_t.sell):
        #     ticker_dummy_ = ticker(identity([1]), identity([2]))
        #     q = quote(limit)
        #     q.lot = lot_size
        #     lifetime = lifetime_t.good_until_cancelled
        #     return limit_order_message(ticker_dummy_, trader_id, side, q, size, lifetime)
        #
        #
        #

        print(f"{self} act({step},{seed}) = {next_event}")
        return next_event

    def update_beliefs(self, p: price, tau):
        self.fcast = self.forecast_norm * (self.weight_fundamental * self.fundamental
                                           + self.weight_technical * self.chartist[self.horizon]
                                           + self.weight_noise * self.noise
                                           )

        self.fcast = max(min(self.fcast, 0.5), -0.5)

        # exponentiate the forecast to get future price forecast
        # note:  this could have a variance adjustment, but it doesn't at the moment
        self.pfcast = price.approximate(float(p) * np.exp(self.fcast + 0.0 * self.v), p.valuation)
