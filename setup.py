#!python
import setuptools
try:
    from setuptools import setup
    from setuptools import Extension
except ImportError:
    from distutils.core import setup
    from distutils.extension import Extension


def parse_version():
    version = []
    with open('version') as f:
        for l in f.readlines():
            version.append(l.split()[-1])
    version_representation = ".".join(map(str,version))
    return version, version_representation


classifiers = [
    "Development Status :: 2 - Pre-Alpha"

    "Programming Language :: C++",

    "Programming Language :: Python :: 3.6",
    "Programming Language :: Python :: 3.7",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3 :: Only",

    "Topic :: Other/Nonlisted Topic",
    "Topic :: Scientific/Engineering",
    "Topic :: Software Development :: Libraries",

    "Intended Audience :: Financial and Insurance Industry",
    "Intended Audience :: Science/Research",

    "License :: OSI Approved :: Apache Software License",

    "Operating System :: Microsoft :: Windows",
    "Operating System :: Unix",
    "Operating System :: MacOS",
]


with open("README.md", "r") as fh:
    long_description = fh.read()


modules = [ Extension("esl.quantity",                   ["esl/quantity.py"])
          , Extension("esl.version",                    ["esl/version.py"])

            ########################################################################################################
            # computation
            , Extension("esl.computation.environment",  ["esl/computation/environment.py"])
            , Extension("esl.computation.timing",       ["esl/computation/timing.py"])

            #   distributed
            , Extension("esl.computation.distributed.mpi_environment", ["esl/computation/distributed/mpi_environment.py"])
            , Extension("esl.computation.distributed.protocol",        ["esl/computation/distributed/protocol.py"
                                                                       ,"esl/computation/distributed/protocol.pxd"
                                                                       ])

            ########################################################################################################
            # data
            , Extension("esl.data.output_base",                 ["esl/data/output_base.py"])
            , Extension("esl.data.output",                      ["esl/data/output.py"])
            , Extension("esl.data.producer",                    ["esl/data/producer.py"])

            ########################################################################################################
            # economics
            , Extension("esl.economics.asset",                  ["esl/economics/asset.py"])
            , Extension("esl.economics.cash",                   ["esl/economics/cash.py"])
            , Extension("esl.economics.commodity",              ["esl/economics/commodity.py"])
            , Extension("esl.economics.currencies",             ["esl/economics/currencies.py"])
            , Extension("esl.economics.currency",               ["esl/economics/currency.py"])
            , Extension("esl.economics.fungibility",            ["esl/economics/fungibility.py"])
            , Extension("esl.economics.iso4217",                ["esl/economics/iso4217.py"])
            , Extension("esl.economics.money",                  ["esl/economics/money.py"])
            , Extension("esl.economics.owner",                  ["esl/economics/owner.py"])
            , Extension("esl.economics.price",                  ["esl/economics/price.py"])
            , Extension("esl.economics.rate",                   ["esl/economics/rate.py"])
            , Extension("esl.economics.tangibility",            ["esl/economics/tangibility.py"])

            #   accounting
            , Extension("esl.economics.accounting.inventory",   ["esl/economics/accounting/inventory.py"])
            , Extension("esl.economics.accounting.standard",    ["esl/economics/accounting/standard.py"])
            #   finance
            , Extension("esl.economics.finance.company",        ["esl/economics/finance/company.py"])
            , Extension("esl.economics.finance.dividend",       ["esl/economics/finance/dividend.py"])
            , Extension("esl.economics.finance.isin",           ["esl/economics/finance/isin.py"])
            , Extension("esl.economics.finance.loan",           ["esl/economics/finance/loan.py"])
            , Extension("esl.economics.finance.security",       ["esl/economics/finance/security.py"])
            , Extension("esl.economics.finance.share",          ["esl/economics/finance/share.py"])
            , Extension("esl.economics.finance.shareholder",    ["esl/economics/finance/shareholder.py"])
            , Extension("esl.economics.finance.stock",          ["esl/economics/finance/stock.py"])
            #   markets
            , Extension("esl.economics.markets.demand_supply_function", ["esl/economics/markets/demand_supply_function.py"])
            , Extension("esl.economics.markets.indication",             ["esl/economics/markets/indication.py"])
            , Extension("esl.economics.markets.iso10383",               ["esl/economics/markets/iso10383.py"])
            , Extension("esl.economics.markets.market",                 ["esl/economics/markets/market.py"])
            , Extension("esl.economics.markets.order_message",          ["esl/economics/markets/order_message.py"])
            , Extension("esl.economics.markets.quote",                  ["esl/economics/markets/quote.py"])
            , Extension("esl.economics.markets.quote_message",          ["esl/economics/markets/quote_message.py"])

            ########################################################################################################
            # geography
            , Extension("esl.geography.country",                ["esl/geography/country.py"])
            , Extension("esl.geography.countries",              ["esl/geography/countries.py"])
            , Extension("esl.geography.iso31661alpha2",         ["esl/geography/iso31661alpha2.py"])
            , Extension("esl.geography.location",               ["esl/geography/location.py"])

            ########################################################################################################
            # law
            , Extension("esl.law.contract",                ["esl/law/contract.py"])
            , Extension("esl.law.government",              ["esl/law/government.py"])
            , Extension("esl.law.iso17442",                ["esl/law/iso17442.py"])
            , Extension("esl.law.jurisdiction",            ["esl/law/jurisdiction.py"])
            , Extension("esl.law.jurisdictions",           ["esl/law/jurisdictions.py"])
            , Extension("esl.law.legal_person",            ["esl/law/legal_person.py"])
            , Extension("esl.law.natural_person",          ["esl/law/natural_person.py"])
            , Extension("esl.law.organization",            ["esl/law/organization.py"])
            , Extension("esl.law.property",                ["esl/law/property.py"])
            , Extension("esl.law.property_collection",     ["esl/law/property_collection.py"])

            ########################################################################################################
            # mathematics
            , Extension("esl.mathematics.interval",        [ "esl/mathematics/interval.py"])
            , Extension("esl.mathematics.rational",        [ "esl/mathematics/rational.py"])
            , Extension("esl.mathematics.variable",        [ "esl/mathematics/variable.py"])

            ########################################################################################################
            # simulation
            , Extension("esl.simulation.parameter.parametrization", ["esl/simulation/parameter/parametrization.py"])
            , Extension("esl.simulation.agent_collection",          ["esl/simulation/agent_collection.py"])
            , Extension("esl.simulation.entity",                    ["esl/simulation/entity.py"])
            , Extension("esl.simulation.identity",                  ["esl/simulation/identity.py"])
            , Extension("esl.simulation.model",                     ["esl/simulation/model.py"])
            , Extension("esl.simulation.time",                      ["esl/simulation/time.py"])
            , Extension("esl.simulation.world",                     ["esl/simulation/world.py"])

            #     parameter
            , Extension("esl.simulation.parameter.parametrization",   ["esl/simulation/parameter/parametrization.py"])
          ]

for module in modules:
    module.cython_directives = {'language_level': "3"}

setuptools.setup(
    name                            = "esl",
    version                         = f"parse_version()[1]}-python",
    author                          = "The Institute for New Economic Thinking at the Oxford Martin School",
    author_email                    = "maarten.scholl@cs.ox.ac.uk",
    description                     = "The Economic Simulation Library",
    long_description                = long_description,
    long_description_content_type   = "text/markdown",
    url                             = "https://github.com/INET-Complexity/ESL",
    packages                        = setuptools.find_packages(),
    classifiers                     = classifiers,
    python_requires                 = ">=3.6",
    setup_requires                  = ["setuptools>=18.0", "cython"],
    ext_modules                     = modules,
)
