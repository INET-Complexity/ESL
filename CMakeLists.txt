CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

################################################################################
# Read and set the library version
################################################################################
FILE(READ "esl/version" version_header)

STRING(REGEX MATCH "[0-9a-zA-Z_]*ESL_VERSION_MAJOR[ \t]*=[ \t]*([0-9]+)"
        _ ${version_header})
SET(version_major ${CMAKE_MATCH_1})

STRING(REGEX MATCH "[0-9a-zA-Z_]*ESL_VERSION_MINOR[ \t]*=[ \t]*([0-9]+)"
        _ ${version_header})
SET(version_minor ${CMAKE_MATCH_1})

STRING(REGEX MATCH "[0-9a-zA-Z_]*ESL_VERSION_REVISION[ \t]*=[ \t]*([0-9]+)"
        _ ${version_header})
SET(version_revision ${CMAKE_MATCH_1})

# Enable new behaviour on this policy so we can set the version automatically.
# enables compatibility with older versions of cmake
CMAKE_POLICY(SET CMP0048 NEW)

IF(APPLE)
    # old version 10.9 for compatibility
    SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
    SET(CMAKE_MACOSX_RPATH 0)

    # If environment settings override this, you can enable force:
    # ADD_COMPILE_OPTIONS(-mmacosx-version-min=10.9)
ENDIF()


PROJECT(ESL VERSION ${version_major}.${version_minor}.${version_revision}
        DESCRIPTION "Economic Simulation Library"
        LANGUAGES CXX)

MESSAGE("ESL Version ${PROJECT_VERSION}")
MESSAGE("Building binary in ${CMAKE_BINARY_DIR}")

################################################################################
#   Project Configuration
################################################################################
SET(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")

SET(CMAKE_CXX_STANDARD              17)
SET(CMAKE_CXX_STANDARD_REQUIRED     ON)
SET(CMAKE_CXX_EXTENSIONS            OFF)

OPTION(CONFIGURATION_SHARED "Build shared library" OFF)

OPTION(WITH_TESTS "Build test cases" ON)


IF(NOT ESL_TARGET_NAME)
    SET(ESL_TARGET_NAME "esl")
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE("CMAKE_BUILD_TYPE not set, defaulting to Release build.")
    SET(CMAKE_BUILD_TYPE    Release)
ENDIF()

IF(MSVC OR MINGW)
    ADD_DEFINITIONS(-D_NO_W32_PSEUDO_MODIFIERS )
ENDIF()

# ignore warnings about unavailable functions in standard library on older MacOS
IF(APPLE)
    ADD_DEFINITIONS(-D_LIBCPP_DISABLE_AVAILABILITY)
ENDIF()

################################################################################
# Caching
################################################################################

FIND_PROGRAM(CCACHE_FOUND ccache)
IF(CCACHE_FOUND)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
ENDIF(CCACHE_FOUND)

################################################################################
# Library optional add-ons
################################################################################
OPTION(WITH_MPI "Enable MPI using Boost.MPI" OFF)
IF(WITH_MPI)
    FIND_PACKAGE(MPI REQUIRED)
    ADD_DEFINITIONS(-DWITH_MPI)
ENDIF()

OPTION(WITH_PYTHON "Enables export of Python bindings using Boost.Python" ON)
IF(WITH_PYTHON)
    ADD_DEFINITIONS(-DWITH_PYTHON)

    IF(SKBUILD)
        # No linking to python
    ELSE()
        ADD_DEFINITIONS(-DBOOST_PYTHON_STATIC_LIB)
    ENDIF()
ENDIF()

OPTION(WITH_QL  "Enables QuantLib" OFF)
IF(WITH_QL)
    ADD_DEFINITIONS(-DWITH_QL)
ENDIF()

################################################################################
# Python bindings configuration
################################################################################
IF(WITH_PYTHON)
    FIND_PACKAGE(PythonLibs 3 REQUIRED)  # for building bindings
    FIND_PACKAGE(PythonInterp 3) # for running tests, not required

    IF(SKBUILD)
        MESSAGE(STATUS "The project is built using scikit-build")
    ELSE()
        MESSAGE(STATUS "scikit-build disabled")
    ENDIF()

    STRING(REPLACE "." ";" VERSION_LIST ${PYTHONLIBS_VERSION_STRING})
    LIST(GET VERSION_LIST 0 ESL_PYTHON_BINDINGS_VERSION_MAJOR)
    LIST(GET VERSION_LIST 1 ESL_PYTHON_BINDINGS_VERSION_MINOR)
    LIST(GET VERSION_LIST 2 ESL_PYTHON_BINDINGS_VERSION_PATCH)
ENDIF()

INCLUDE_DIRECTORIES(SYSTEM ${PYTHON_INCLUDE_DIRS})

INCLUDE(GNUInstallDirs)

################################################################################
# In this section we set up the Boost libraries,
# and inter-dependencies between Boost MPI and Python
################################################################################
LIST(APPEND Boost_LIBRARIES_DEPENDENCIES
        program_options
        date_time
        serialization
        unit_test_framework
        thread
        )

# If Python is enabled, we add Boost.Python of the appropriate version
IF(WITH_PYTHON)
    MESSAGE("################################################################################")
    MESSAGE("Compiling with Python ${ESL_PYTHON_BINDINGS_VERSION_MAJOR}.${ESL_PYTHON_BINDINGS_VERSION_MINOR}:")

    #IF(ESL_PYTHON_BINDINGS_VERSION_MINOR LESS 8)
    #    LIST(APPEND Boost_LIBRARIES_DEPENDENCIES
    #            python${ESL_PYTHON_BINDINGS_VERSION_MAJOR}${ESL_PYTHON_BINDINGS_VERSION_MINOR})
    #ELSE()
    LIST(APPEND Boost_LIBRARIES_DEPENDENCIES python38)
    #ENDIF()

    IF(NOT CMAKE_BUILD_TYPE EQUAL "Release")
        MESSAGE("include: \t" ${PYTHON_INCLUDE_DIRS})
        MESSAGE("library: \t" ${PYTHON_LIBRARY})
        MESSAGE("libraries: \t" ${PYTHON_LIBRARIES})
    ENDIF()
ENDIF()

# If using the Message Passing Interface, enable MPI
IF(WITH_MPI)
    LIST(APPEND Boost_LIBRARIES_DEPENDENCIES mpi)

    # We can expose the python bindings for MPI as well, to permit direct
    # And enable MPI_Python
    #IF(WITH_PYTHON)
    #    LIST(APPEND Boost_LIBRARIES_DEPENDENCIES mpi_python)
    #ENDIF()
ENDIF()

################################################################################
#   Collect native code source files
################################################################################
file(GLOB_RECURSE SOURCE_FILES "esl/**.cpp" "esl/**.hpp")

################################################################################
#
################################################################################
LIST(APPEND ALL_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR} SYSTEM)

IF(MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS " -static")
    SET(Boost_USE_STATIC_RUNTIME     ON) # not necessary on linux/macOS
    SET(Boost_USE_STATIC_LIBS        ON)
ELSE()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON) # MINGW?
ENDIF()

SET(Boost_USE_MULTITHREADED      ON)









IF(FALSE) # MSVC AND IDE BUILD
    # Install-Package gsl-msvc-x64 -Version 2.4.0.8788
    # Install-Package boost -Version 1.72.0
    # Install-Package boost_program_options-vc140 -Version 1.72.0
    #

    MESSAGE("################################################################################")
    MESSAGE("MSVC build detected, please use NuGet package manager to install dependencies if not yet satisfied: ")
    LIST(APPEND VS_DEPENDENCIES "")


    # TODO: we hardcode it for github actions, but we should make this generic
    LIST(APPEND ALL_INCLUDE_DIRECTORIES "C:/adept")

    # Github Actions setup:
    # C:\Users\runneradmin\.nuget\packages\

    SET(MSVC_GSL_VERSION "2.4.0.8788")
    MESSAGE("\tInstall-Package gsl-msvc-x64 -Version ${MSVC_GSL_VERSION}")
    LIST(APPEND ALL_INCLUDE_DIRECTORIES "C:/Users/runneradmin/.nuget/packages/gsl-msvc-x64.${MSVC_GSL_VERSION}/build/native")
    LIST(APPEND VS_DEPENDENCIES "gsl-msvc-x64_${MSVC_GSL_VERSION}") #MESSAGE("gsl-msvc-x64_${MSVC_GSL_VERSION}")
    LIST(APPEND GSL_LIBRARY_DIRS "C:/Users/runneradmin/.nuget/packages/gsl-msvc-x64.${MSVC_GSL_VERSION}/build/native/lib/x64/Release")


    SET(MSVC_VERSION_PACKAGE_MANAGER "vc142")
    SET(MSVC_BOOST_VERSION "1.72.0")
    LIST(APPEND VS_DEPENDENCIES "boost_${MSVC_BOOST_VERSION}")
    LIST(APPEND ALL_INCLUDE_DIRECTORIES "C:/Users/runneradmin/.nuget/packages/boost.1.72.0.0/lib/native/include")

    # to be compliant, we need to set most of the following variables:
    # Boost_LIBRARIES
    LIST(APPEND Boost_LIBRARIES )

    FOREACH(bll ${Boost_LIBRARIES_DEPENDENCIES})
        MESSAGE("\tInstall-Package boost_${bll}-${MSVC_VERSION_PACKAGE_MANAGER} -Version ${MSVC_BOOST_VERSION}")
        #MESSAGE("\tboost_${bll}_${MSVC_BOOST_VERSION}")
        LIST(APPEND VS_DEPENDENCIES "boost_${bll}-${MSVC_VERSION_PACKAGE_MANAGER}_${MSVC_BOOST_VERSION}")

    ENDFOREACH()

    # Boost_FOUND
    SET(Boost_FOUND TRUE)
    #Boost_INCLUDE_DIRS
    SET(Boost_INCLUDE_DIRS "C:/Users/runneradmin/.nuget/packages/boost.1.72.0.0/lib/native/include/")
    #Boost_LIBRARY_DIRS
    LIST(APPEND Boost_LIBRARY_DIRS)
    #Boost_LIBRARIES
    FOREACH(bll ${Boost_LIBRARIES_DEPENDENCIES})
        LIST(APPEND Boost_LIBRARY_DIRS "C:/Users/runneradmin/.nuget/packages/boost_${bll}-${MSVC_VERSION_PACKAGE_MANAGER}.${MSVC_BOOST_VERSION}.0/lib/native/")
        # Boost_<C>_FOUND
        SET("Boost_${bll}_FOUND" TRUE)
    ENDFOREACH()

    MESSAGE("add library gsl.lib cblas.lib")
    #ADD_LIBRARY("${bll}-vc142-mt-x64-1_72.lib" ALIAS "Boost::${bll}")
    LIST(APPEND Boost_LIBRARIES gsl.lib cblas.lib)

    FOREACH(bll ${Boost_LIBRARIES_DEPENDENCIES})
        MESSAGE("add library libboost_${bll}-vc142-mt-s-x64-1_72.lib")
        LIST(APPEND Boost_LIBRARIES "libboost_${bll}-vc142-mt-s-x64-1_72.lib")
    ENDFOREACH()

    #
    #Boost_<C>_LIBRARY
    #Boost_VERSION
    #Boost_LIB_VERSION
    #Boost_MAJOR_VERSION
    #Boost_MINOR_VERSION
    #Boost_SUBMINOR_VERSION
    #Boost_LIB_DIAGNOSTIC_DEFINITIONS (Windows)
ELSE()
    FIND_PACKAGE(Boost 1.53 COMPONENTS ${Boost_LIBRARIES_DEPENDENCIES} REQUIRED)
ENDIF()

#FIND_PACKAGE(Boost 1.53 COMPONENTS ${Boost_LIBRARIES_DEPENDENCIES} REQUIRED)

########################################################################################################################
# Optimized linear algebra and scientific computing routines
MESSAGE("################################################################################")

IF(MSVC)
    SET(BLA_STATIC ON) # On windows, we will statically link a free blas package
ENDIF()

FIND_PACKAGE(BLAS) # not required
IF(BLAS_FOUND)
    FIND_PACKAGE(LAPACK) # also not required but nice to have
ENDIF()


# GSL is required for optimisation routines

FIND_PACKAGE(GSL REQUIRED)

################################################################################
MESSAGE("----------------------------------------")
MESSAGE("profile  " ${CMAKE_BUILD_TYPE})
MESSAGE("compiler " ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION} " (" ${CMAKE_CXX_COMPILER} ")")
MESSAGE("----------------------------------------")

IF(MSVC)
    #TARGET_COMPILE_OPTIONS(${ESL_TARGET_NAME} PRIVATE "/MP")

ELSE()
    #ADD_COMPILE_OPTIONS(-m64) # should be the default with modern gccs

    SET(CMAKE_CXX_FLAGS "-Wall -Wextra")

    SET(CMAKE_C_FLAGS_DEBUG "-g")#   --coverage -fprofile-arcs -ftest-coverage") #  -fanalyzer
    SET(CMAKE_CXX_FLAGS_DEBUG "-g")#    --coverage -fprofile-arcs -ftest-coverage")

    SET(CMAKE_C_FLAGS_RELEASE "-O3")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

    #set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
        INCLUDE(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
        ADD_DEFINITIONS(-DESL_RELEASE)
        IF(COMPILER_SUPPORTS_MARCH_NATIVE)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        ENDIF()
    ELSE()

    ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)
ENDIF()


ADD_DEFINITIONS(-DADEPT_RECORDING_PAUSABLE)
ADD_DEFINITIONS(-DADEPT_STORAGE_THREAD_SAFE)
# Used to benchmark improvements using automatic differentiation
#ADD_DEFINITIONS(-DADEPT_NO_AUTOMATIC_DIFFERENTIATION)

################################################################################
# Enables ADD_TEST
################################################################################

ENABLE_TESTING()

################################################################################
# Add include directories
################################################################################

IF(WITH_MPI)
    list(APPEND ALL_INCLUDE_DIRECTORIES ${MPI_INCLUDE_PATH})
ENDIF()
IF(WITH_PYTHON)
    list(APPEND ALL_INCLUDE_DIRECTORIES ${PYTHON_INCLUDE_DIRS})
ENDIF()

IF(MSVC) # for now, assume all builds on windows are done using MSYS2 64 bit
    # LIST(APPEND ALL_INCLUDE_DIRECTORIES "C:/msys64/usr/include")
ENDIF()

INCLUDE_DIRECTORIES(SYSTEM ${ALL_INCLUDE_DIRECTORIES})
################################################################################
# Include directories
################################################################################

IF(MSVC)
    LIST(APPEND ALL_LINKED_LIBRARIES)
ELSEIF(MINGW)
    LIST(APPEND ALL_LINKED_LIBRARIES -static pthread -lgsl -lgslcblas -lm ) #GSL::gsl)
ELSE()
    LIST(APPEND ALL_LINKED_LIBRARIES pthread GSL::gsl)
ENDIF()

IF(MSVC OR MINGW)
    LIST(APPEND ALL_LINKED_LIBRARIES)
ELSE()
    LIST(APPEND ALL_LINKED_LIBRARIES dl util)
ENDIF()

IF(WITH_PYTHON)

    # pep-0513/#libpythonx-y-so-1
    IF(SKBUILD AND NOT APPLE)
        # No linking to python
    ELSE()
        LIST(APPEND ALL_LINKED_LIBRARIES ${PYTHON_LIBRARIES})
    ENDIF()
ENDIF()

IF(WITH_MPI)
    LIST(APPEND ALL_LINKED_LIBRARIES ${MPI_CXX_LIBRARIES} )
ENDIF()

IF(WITH_QL)
    LIST(APPEND ALL_LINKED_LIBRARIES QuantLib ${QuantLib_LIBRARIES})
ENDIF()

IF(BLAS_FOUND)
    LIST(APPEND ALL_LINKED_LIBRARIES ${BLAS_LIBRARIES} GSL::gslcblas)
    IF(LAPACK_FOUND)
        LIST(APPEND ALL_LINKED_LIBRARIES ${LAPACK_LIBRARIES})
    ENDIF()
ENDIF()

IF(MSVC)
#    MESSAGE("LINKING WITH")
#    MESSAGE(${ALL_LINKED_LIBRARIES})
    LINK_LIBRARIES(${ALL_LINKED_LIBRARIES})
ELSEIF(MINGW)
    LINK_LIBRARIES(${ALL_LINKED_LIBRARIES} -static -static-libgcc -static-libstdc++)
ELSE()
    LINK_LIBRARIES(${ALL_LINKED_LIBRARIES} dl)
ENDIF()

################################################################################
# This needs to be at this position, because for MSVC we need to issue a LINK_DIRECTORIES
# call before this to get it right

IF(CONFIGURATION_SHARED)
    ADD_LIBRARY(${ESL_TARGET_NAME} SHARED ${SOURCE_FILES} )

    SET_TARGET_PROPERTIES(${ESL_TARGET_NAME} PROPERTIES PREFIX "lib")

    IF(MSVC)
        # multi-processor compilation
        TARGET_COMPILE_OPTIONS(${ESL_TARGET_NAME} PRIVATE "/MP")
        SET(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
        SET_PROPERTY(TARGET ${ESL_TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
    ENDIF()

    IF(CMAKE_BUILD_TYPE MATCHES Debug)
        # TARGET_COMPILE_DEFINITIONS(${ESL_TARGET_NAME} PUBLIC ESL_RELEASE=0)
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        TARGET_COMPILE_DEFINITIONS(${ESL_TARGET_NAME} PUBLIC ESL_RELEASE=1)
    endif()

    IF(MSVC)
        TARGET_LINK_LIBRARIES(${ESL_TARGET_NAME} ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES})
    ELSEIF(MINGW)
        LINK_DIRECTORIES(Boost_LIBRARY_DIRS GSL_LIBRARY_DIRS)
        TARGET_LINK_LIBRARIES(${ESL_TARGET_NAME} ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES} -static -static-libgcc -static-libstdc++)
    ELSE()
        TARGET_LINK_LIBRARIES(${ESL_TARGET_NAME} ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES} dl)
    ENDIF()


    ################################################################################
    # Copy library into testing folder, so it can be linked against by Python tests
    ################################################################################
    ADD_CUSTOM_COMMAND(TARGET ${ESL_TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${ESL_TARGET_NAME}>
            ${CMAKE_BINARY_DIR}/test/esl/$<TARGET_FILE_NAME:${ESL_TARGET_NAME}>)
ENDIF()

################################################################################
#   Add tests
################################################################################

IF(WITH_TESTS AND CONFIGURATION_SHARED)
    FILE(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/test_*.cpp)
    FOREACH(test_src ${TEST_SRCS})
        GET_FILENAME_COMPONENT(test_name ${test_src} NAME_WE)

        ADD_EXECUTABLE(${test_name} ${test_src} ${SOURCE_FILES})
        TARGET_LINK_LIBRARIES(${test_name} ${Boost_LIBRARIES} ${ESL_TARGET_NAME})
        SET_TARGET_PROPERTIES(${test_name} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/test/)

        # test_mpi_* and test_mpi_python_* need to be run using a MPI job runner
        IF(${test_name} MATCHES "^(test_mpi_)")
            IF(WITH_MPI)
                IF(WITH_PYTHON AND ${test_name} MATCHES "^(test_mpi_python_)")
                    MESSAGE("\t MPI PYTHON TEST" ${test_name})
                    ADD_TEST(NAME ${test_name}
                            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/python/test/
                            COMMAND ${CMAKE_BINARY_DIR}/python/test/${test_name})
                ELSE()
                    MESSAGE("\t MPI TEST" ${test_name})
                    ADD_TEST(NAME ${test_name}
                            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
                            COMMAND mpirun -np 2 ${CMAKE_BINARY_DIR}/test/${test_name})
                ENDIF()
            ENDIF()
        ELSEIF(${test_name} MATCHES "^(test_python_)")
            MESSAGE("\t PYTHON TEST" ${test_name})
            ADD_TEST(NAME ${test_name}
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/python/test/
                    COMMAND ${CMAKE_BINARY_DIR}/python/test/${test_name})
        ELSE()
            MESSAGE("\t C++ TEST " ${test_name})
            ADD_TEST(NAME ${test_name}
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
                    COMMAND ${CMAKE_BINARY_DIR}/test/${test_name})
        ENDIF()

    ENDFOREACH(test_src)
ENDIF()


IF(WITH_PYTHON)
    ####################################################################################################################
    # create python modules
    # the requirement a file to be parsed as a python module is that it is a uniquely named .cpp file
    # with python_module as prefix of the filename
    FILE(GLOB_RECURSE ESL_PYTHON_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} esl/**python_module**.cpp)
    FOREACH (source ${ESL_PYTHON_SOURCE})
        GET_FILENAME_COMPONENT(module_name ${source} NAME_WE)
        GET_FILENAME_COMPONENT(module_path ${source} DIRECTORY)

        GET_FILENAME_COMPONENT(python_module_name ${module_path} NAME)

        LINK_LIBRARIES(${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES})

        PYTHON_ADD_MODULE(_${python_module_name} ${source} ${SOURCE_FILES})

        IF(python_module_name STREQUAL "esl")
            SET(module_path_modified ".")
        ELSE()
            STRING(SUBSTRING  ${module_path} 4 -1 module_path_modified)
        ENDIF()

        MESSAGE("PYTHON MODULE " _${python_module_name} ":\t\t " ${module_path_modified} "\t\t" ${source})
        INSTALL(TARGETS _${python_module_name} DESTINATION ${module_path_modified})

        SET(ignoreMe "${SKBUILD}")
    ENDFOREACH ()

    ####################################################################################################################
    # create python tests
    FILE(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/test_*.py)
    FOREACH(test_src ${TEST_SRCS})
        GET_FILENAME_COMPONENT(pytest_name ${test_src} NAME_WE)
        SET(pytest_name "${pytest_name}_python")
        MESSAGE("PYTEST " ${pytest_name})
        FILE(COPY ${test_src} DESTINATION ${CMAKE_BINARY_DIR}/python/test/)
    ENDFOREACH(test_src)

    ADD_CUSTOM_TARGET(pytest ALL COMMENT "pytest")

    #SET_TARGET_PROPERTIES(pytest PROPERTIES
    #    RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/python/test/)

    #IF(PYTHONINTERP_FOUND)
    #    ADD_TEST(NAME pytest
    #            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/python/test/
    #            COMMAND ${PYTHON_EXECUTABLE} -m pytest -r a -v ${CMAKE_BINARY_DIR}/python/test/)
    #ENDIF()
ENDIF()