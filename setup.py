import sys
import os


try:
    from skbuild import setup
except ImportError:
    print('Please update pip, you need pip 10 or greater,\n'
          ' or you need to install the PEP 518 requirements in pyproject.toml yourself', file=sys.stderr)
    raise

def read_version():
    try:
        with open("esl/version", "r") as source_version:
            lines = source_version.readlines()
            version = []
            for line in lines:
                if "ESL_VERSION_" in line:
                    number = int(line.split(";")[0].split('=')[1].strip())
                    version.append(number)

            return tuple(version)
    except:
        raise ValueError("Can not read esl/version file.")

def get_packages():

    #for root, dirs, files in os.walk('notes'):
    #    print(root, dirs, files)
    pass


setup(
    name                = "esl",
    version             = ".".join(map(str, read_version())),
    description         = "Economic Simulation Library ",
    author              = "Maarten P. Scholl",
    #author_email        =
    license             = "Apache License 2.0",
    packages            = [ 'esl'
                          , 'esl.computation'
                          , 'esl.computation.distributed'
                          , 'esl.mathematics'
                          , 'esl.simulation'
                          ],

    package_dir         = { 'esl': 'esl'
                          , 'esl.computation': 'esl/computation'
                          , 'esl.computation.distributed': 'esl/computation/distributed'
                          , 'esl.mathematics': 'esl/mathematics'
                          , 'esl.simulation': 'esl/simulation'
                          },
    cmake_install_dir   = 'esl'
)