from esl.economics import *
from esl.simulation import *
from esl.economics.markets import *
from esl.economics.markets.order_book import *


import numpy as np
import numpy.random as rnd
#   Chiarella Iori 2002 modules
from agent import *

USD = currencies.USD


print(esl.version())



parameters = esl.simulation.parameter.parametrization()

runType = 0
if runType == 0:
    sigmaF = 0.
    sigmaM = 0.
    sigmaN = 1.
elif runType == 1:
    sigmaF = 1.
    sigmaM = 0.
    sigmaN = 1.
else:# runType == 2:
    sigmaF = 1.
    sigmaM = 10.
    sigmaN = 1.

parameters.add_constant('start',            np.uint64(0))       # default
parameters.add_constant('tinit',            np.uint64(10))      # 1_000
parameters.add_constant('end',              np.uint64(50))      # 50_000
parameters.add_constant('lmin',             np.uint64(2))       # 5
parameters.add_constant('lmax',             np.uint64(5))       # 50
parameters.add_constant('deltaP',           np.double(0.1))     #
parameters.add_constant('sigmae',           np.double(0.01))    #
parameters.add_constant('kMax',             np.double(0.5))     #
parameters.add_constant('tau',              np.uint64(50))      #
parameters.add_constant('deltaT',           np.uint64(100))     # time length for ticks per time period
parameters.add_constant('number_agents',    np.uint64(5))      # 1_000
parameters.add_constant('fundamental',      price.approximate(1_000.00, USD))      # price fundamental

fundamental = price.approximate(1_000.00, USD) # price fundamental?

########################################################################################################################


environment = esl.computation.environment()


class ChiarellaIori(esl.simulation.model):
    def __init__(self, env, params):
        super().__init__(env, params)
        np.random.seed(parameters['sample'])


        self.prices = np.ones(parameters['end'] + 1) * float(fundamental)
        self.ret     = np.zeros(parameters['end'] + 1)
        self.totalV = np.zeros(parameters['end'] + 1)
        self.rprice = np.zeros((parameters['end'] + 1) // 100)

        self.agent_orders = dict()
        self.agentList = []

        self.agent_identifiers = []



    def create_agents(self):
        """

        :return:
        """
        wakeup_times = dict()
        for t in range(parameters['tinit'], parameters['end']):
            i = np.random.randint(0, parameters['number_agents'])
            wakeup_times[i] = wakeup_times.get(i, []) + [t]

        print()
        print(wakeup_times)
        print()

        for i in range(parameters['number_agents']):
            ai = identity([0, i])
            a = TradingAgent(ai
                      , sigmaF
                      , sigmaM
                      , sigmaN
                      , parameters['sigmae']
                      , parameters['kMax']
                      , parameters['lmin']
                      , parameters['lmax']
                      , parameters['tau']
                      , fundamental
                      , wakeup_times.get(i, [])
                      )

            self.agentList.append(a)
            self.agents.activate(a)
            self.agent_identifiers.append(id)


    def create_market(self):

        lot_size = 1

        lower_price = price(0, USD)
        upper_price = price(1400_00, USD)

        # we will create a simple equities central order book
        self.order_book = static_order_book( quote(lower_price)
                                           , quote(upper_price)
                                           , 100000)

        esl.log(f"order book valid quotes: [{quote(lower_price)}, {quote(upper_price)}]")

        self.prices = float(fundamental) * np.random.normal(1, 0.001, size=self.prices.shape[0])
        self.ret    = np.random.normal(0, 0.001, size=self.ret.shape[0])

    def initialize(self):
        """
        Sets up our model, ready to run.
        @detail Inherited from esl.simulation.model
        :return:
        """

        print(f"Populating our economy with {parameters['number_agents']} agents")
        self.create_market()
        self.create_agents()


model = ChiarellaIori(environment, parameters)
np.random.seed(parameters['sample'])

environment.run(model)

import sys
sys.exit()



