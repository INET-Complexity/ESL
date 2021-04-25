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

import computation
import data
import economics
import geography
import interaction
import law
import mathematics
import simulation
