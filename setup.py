import os
import pathlib
import sys
import subprocess
import typing

# The relative path of the source code to the setup file
source_directory = 'esl'

try:
    # Attempt to load skbuild, which is necessary to build this python package
    from skbuild import setup
except ImportError:
    error_message = 'Please update pip, you need pip 10 or greater, or install the skbuild package, or install the ' \
                    'PEP 518 requirements in pyproject.toml '
    print(error_message, file=sys.stderr)
    raise ImportError(error_message)


def read_version():
    """
    Reads the library version number from the unified version files, esl/version
    :return:    Version tuple, in (major, minor, patch) format.
    """
    try:
        with open(f"{source_directory}/version", "r") as source_version:
            lines = source_version.readlines()
            version = []
            for line in lines:
                if "ESL_VERSION_" in line:
                    number = int(line.split(";")[0].split('=')[1].strip())
                    version.append(number)

            return tuple(version)
    except:
        raise ValueError(f"Can not read {source_directory}/version file.")


def read_commit() -> typing.Optional[str]:
    """
        Gets a commit
    :return:
    """
    try:
        return subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).strip().decode()
    except subprocess.CalledProcessError:
        # likely git not installed, or not authorized to use
        return None


def get_packages():
    """
    Walks the source directory to find python modules. A module has an __init__.py file in the directory.
    :return:    A dictionary mapping package names to subdirectories, e.g. {'esl.simulation': 'esl/simulation'}
    """
    packages = dict()
    for subdirectory, dirs, files in os.walk(source_directory):
        for file in files:
            entry = subdirectory + os.sep + file
            if entry.endswith("__init__.py"):
                packages[subdirectory.replace('/', '.')] = subdirectory
    return packages


packages = get_packages()

commit = None #read_commit()

# Set up the package, together with metadata that will be visible on package repositories such as Pypi
setup(
    name                = 'eslpy',

    version             = '.'.join(map(str, read_version())) + ('' if commit is None else '-' + commit),
    description         = 'Python package for the Economic Simulation Library (https://github.com/INET-Complexity/ESL/)',

    # This loads the README file, and allows repositories such as Pypi to render the readme using markdown
    long_description    = (pathlib.Path(__file__).parent / "README.md").read_text(),
    long_description_content_type="text/markdown",

    author              = 'Maarten P. Scholl',
    author_email        = 'maarten.scholl@cs.ox.ac.uk',

    classifiers         = [ 'Development Status :: 3 - Alpha',
                            'License :: OSI Approved :: Apache Software License',
                            'Programming Language :: Python',
                            'Programming Language :: C++',
                            'Topic :: Scientific/Engineering',
                            'Intended Audience :: Science/Research',
                          ],

    license             = 'Apache License 2.0',

    packages            = list(packages.keys()),
    package_dir         = packages,

    package_data        = { ''   : []
                          , 'esl': ['version']
                          },

    cmake_install_dir   = 'esl'
)
