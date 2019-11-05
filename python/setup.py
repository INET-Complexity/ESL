import setuptools


def parse_version():
    version = [0,0,0]
    with open("../esl/version.hpp") as version_header:
        for l in version_header.readlines():
            if 'ESL_VERSION_' in l:
                number = int(l.split('=')[1].split(';')[0].strip())
                if 'ESL_VERSION_MAJOR' in l:
                    version[0] = number
                elif 'ESL_VERSION_MINOR' in l:
                    version[1] = number
                elif 'ESL_VERSION_REVISION' in l:
                    version[2] = number

    version_representation = '.'.join(version)
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


with open("../README.md", "r") as fh:
    long_description = fh.read()


setuptools.setup(
    name                            = "esl-maartenscholl",
    version                         = "{version_representation}",
    author                          = "The Institute for New Economic Thinking at the Oxford Martin School",
    author_email                    = "maarten.scholl@cs.ox.ac.uk",
    description                     = "A small example package",
    long_description                = long_description,
    long_description_content_type   = "text/markdown",
    url                             = "https://github.com/INET-Complexity/ESL",
    packages                        = setuptools.find_packages(),
    classifiers                     = classifiers,
    python_requires                 = '>=3.6',
)
