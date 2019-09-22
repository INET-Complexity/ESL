![esl project logo](https://raw.githubusercontent.com/INET-Complexity/ESL/master/project/logo/logo_card.png)


# ESL
The Economic Simulation Library (ESL) provides an extensive collection of tools to develop, test, analyse and calibrate economic and financial agent-based models. The library is designed to take advantage of different computer architectures. In order to facilitate rapid iteration during model development the library can use parallel computation. Economic models developed using the library can be deployed into large-scale distributed computing environments when working with large model instances and datasets. It provides routines to set up large-scale sampling experiments during the analysis and calibration process.

The library is developed to be used from Python, and is developed in C++. 

# Features

## Models

### Agents


### Market models

The library provides multiple market mechanisms that use the same messaging framework, meaning market mechanisms can to a limited extended be swapped and compared within the same economic model. We aim to provide simple models that allow, for example, to convert directly supply- and demand functions to an execution strategy, so that a Walrasian price-setters can be compared to order-driven markets such as limit order books. 

* Walrasian price-setter, supporting finding clearing prices for one or more for tradeable assets simulaneously.

* Shapley-Shubik trading-posts. *Trade Using One Commodity as a Means of Payment*. Shapley, L. and Shubik, M. **Journal of Political Economy** 1977 vol: 85 (5) pp: 937-968.

* Limit order books


# Examples

As part of the first release, we want to provide at least the following models as examples. 

* Schelling segregation model
* INET Long-Term Impact Market Ecology model
* INET Housing model

## Testing

The library uses unit testing on both the native library (Boost.Test) and the Python bindings (pytest and Hypothesis).

[![Build Status](https://travis-ci.org/INET-Complexity/ESL.svg?branch=master)](https://travis-ci.org/INET-Complexity/ESL)
