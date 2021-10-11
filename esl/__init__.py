#   \file   init__.py
# 
#   \brief
# 
#   \authors    Maarten P. Scholl
#   \date       2017-11-24
#   \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
#               Oxford Martin School, University of Oxford
# 
#               Licensed under the Apache License, Version 2.0 (the "License");
#               you may not use this file except in compliance with the License.
#               You may obtain a copy of the License at
# 
#                   http://www.apache.org/licenses/LICENSE-2.0
# 
#               Unless required by applicable law or agreed to in writing,
#               software distributed under the License is distributed on an "AS
#               IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
#               express or implied. See the License for the specific language
#               governing permissions and limitations under the License.
# 
#               You may obtain instructions to fulfill the attribution
#               requirements in CITATION.cff
#
def initialize_library():
    """
    This function initializes the library by loading dependencies.
    :return:
    """
    import os
    import platform

    bin_dir = os.path.abspath(os.path.dirname(__file__))
    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
    libs_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir, 'eslpy.libs'))

    #print(f"root_dir {root_dir}")
    #print(f"libs_dir {libs_dir}")

    def read_version():
        """
        Reads the library version number from the unified version files, esl/version
        :return:    Version tuple, in (major, minor, patch) format.
        """
        try:
            with open(os.path.join(bin_dir, "version"), "r") as source_version:
                lines = source_version.readlines()
                version = []
                for line in lines:
                    if "ESL_VERSION_" in line:
                        number = int(line.split(";")[0].split('=')[1].strip())
                        version.append(number)

                return tuple(version)
        except:
            raise ValueError(f"Can not read version file.")

    version = read_version()
    #print(f"library version {version}")

    # On windows, we encounter some issues loading libraries, so we load them manually here.
    # The load-order-eslpy file, and the 'os.add_dll_directory(libs_dir)' call are assumed to be added
    # by the delvewheel tool.
    if 'windows' == platform.system().lower():
        from ctypes import WinDLL
        import warnings

        with warnings.catch_warnings():
            with open(os.path.join(libs_dir, f".load-order-eslpy-{version[0]}.{version[1]}.{version[2]}")) as file:
                load_order = file.read().split()
            # print(f"load order = {load_order}")
            for lib in load_order:
                # print(f"\tloading {os.path.join(libs_dir, lib)}")
                WinDLL(os.path.join(libs_dir, lib))


initialize_library()   # initialize library before loading submodules
del initialize_library # remove the function, removing all traces of initialization from the library interface

# This loads the binary library containing all submodules
from esl._esl import *

import esl.computation
import esl.data
import esl.economics
import esl.geography
import esl.interaction
import esl.law
import esl.mathematics
import esl.simulation

import numpy as np


def log(item: object, level: esl.data.severity = esl.data.severity.trace):
    """
        Prints the item to the log file. Used for human readable feedback to modelers and developers -
        for model output, see esl.data.output.
    :param item:    the Python object to be printed. Uses `str(item)`, and thus expects that item.__str__() is defined
    :param level:   One value from `esl.data.severity` = {'trace', 'notice', 'warning', 'error'}
    :return:        void
    """
    import inspect
    esl.data.log_detailed(level, item, inspect.stack()[1].function, inspect.stack()[1].filename, inspect.stack()[1].lineno)


def add_constant_parameter_helper(self: esl.simulation.parameter.parametrization, name, value):
    """
    Adds a constant (scalar) parameter to the models parameters.
    :param self:
    :param name:    the name of the parameter, must be unique, otherwise overwrites previous values
    :param value:   the default value of the parameter
    :return:
    """
    if isinstance(value, float):
        p = esl.simulation.parameter.constant_double(float(value))
    elif isinstance(value, int):
        p = esl.simulation.parameter.constant_int64(int(value))
    elif isinstance(value, esl.economics.price):
        p = esl.simulation.parameter.constant_price(value)
    elif hasattr(value, 'dtype') and isinstance(value.dtype, np.dtype):
        # this requires that `value` is a numpy dtype
        if np.issubdtype(value, np.signedinteger):
            p = esl.simulation.parameter.constant_int64(int(value))
        elif np.issubdtype(value, np.integer):
            p = esl.simulation.parameter.constant_uint64(int(value))
        elif np.issubdtype(value, np.floating):
            p = esl.simulation.parameter.constant_double(float(value))
        else:
            raise TypeError(f"Unsupported numpy parameter type: {type(value)}")
    else:
        raise TypeError(f"Unsupported parameter type: {type(value)}")

    self.values[name] = p


setattr(esl.simulation.parameter.parametrization, 'add_constant', add_constant_parameter_helper)


def add_closed_interval_parameter_helper(self: esl.simulation.parameter.parametrization, name, value):
    """
    Adds a closed interval of possible parameter values
    :param self:
    :param name:
    :param value:
    :return:
    """
    pass


setattr(esl.simulation.parameter.parametrization, 'add_constant', add_constant_parameter_helper)



def simple_experiment(parameters = esl.simulation.parameter.parametrization()):
    """
        Sets up an experiment with a single model. The model is empty except for the minimum of required
        parameters and entities. The parameters are:
            start   (default: 0)            Point in time at which the simulation starts.
            end     (default: 1)            Point in time at which the simulation ends, inclusive.
            sample  (default: 0)            For stochastic models, the sample identifier, effectively the random number seed.
            threads (default: >= 1)         Number of threads to use in the simulation of one model run. Uses C++'s
                                            hardware_concurrency() function to set the default value, and defaults to a
                                            single thread if this method can not determine the number of concurrent threads.
                                            https://en.cppreference.com/w/cpp/thread/thread/hardware_concurrency
            verbosity (default: 1)          how frequently to print updates on the simulation progress

        The default single-threaded environment is used to run the model.
        Convenience functions and syntactic sugar.
    :return: model, parameters and environment objects
    """
    environment = esl.computation.environment()
    model = esl.simulation.model(environment, parameters)

    # set the seed, in case our user forgets to set the seed for the initalization stage
    np.random.seed(parameters['sample'])

    # TODO: add experiment class to do a single run
    return model, environment

