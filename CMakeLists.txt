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

PROJECT(ESL VERSION ${version_major}.${version_minor}.${version_revision}
        DESCRIPTION "Economic Simulation Library" 
        LANGUAGES CXX)

MESSAGE("ESL Version ${PROJECT_VERSION}")
MESSAGE("building binary in ${CMAKE_BINARY_DIR}")

################################################################################
# cmake configuration

CMAKE_MINIMUM_REQUIRED(VERSION 3.15) # 3.9.3)

SET(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")

SET(CMAKE_CXX_STANDARD              17)
SET(CMAKE_CXX_STANDARD_REQUIRED     ON)
SET(CMAKE_CXX_EXTENSIONS            OFF)

# this target is the main library
IF(NOT ESL_TARGET_NAME)
    SET(ESL_TARGET_NAME     "esl")
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE("CMAKE_BUILD_TYPE not set, defaulting to Release build.")
    SET(CMAKE_BUILD_TYPE    Release)
ENDIF()

IF(MSVC OR MINGW)
    ADD_DEFINITIONS(-D_NO_W32_PSEUDO_MODIFIERS )
ENDIF()
################################################################################
#INCLUDE(cotire)

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
    ADD_DEFINITIONS(-DBOOST_PYTHON_STATIC_LIB)
ENDIF()

OPTION(WITH_QL  "Enables QuantLib" OFF)
IF(WITH_QL)
    ADD_DEFINITIONS(-DWITH_QL)
ENDIF()

################################################################################
IF(WITH_PYTHON)
    FIND_PACKAGE(PythonLibs 3.7 REQUIRED)  # for building bindings
    FIND_PACKAGE(PythonInterp 3) # for running tests, not required

    IF(SKBUILD)
        MESSAGE(STATUS "The project is built using scikit-build")
    ELSE()
        #MESSAGE(STATUS "scikit-build disabled")
    ENDIF()

    STRING(REPLACE "." ";" VERSION_LIST ${PYTHONLIBS_VERSION_STRING})
    LIST(GET VERSION_LIST 0 ESL_PYTHON_BINDINGS_VERSION_MAJOR)
    LIST(GET VERSION_LIST 1 ESL_PYTHON_BINDINGS_VERSION_MINOR)
    LIST(GET VERSION_LIST 2 ESL_PYTHON_BINDINGS_VERSION_PATCH)
ENDIF()

################################################################################
# In this section we set up the Boost libraries,
# and inter-dependencies between Boost MPI and Python
LIST(APPEND Boost_LIBRARIES_DEPENDENCIES
            program_options
            date_time 
            serialization 
            unit_test_framework
            thread
    )

IF(WITH_PYTHON)
    MESSAGE("################################################################################")
    MESSAGE("Compiling with Python ${ESL_PYTHON_BINDINGS_VERSION_MAJOR}.${ESL_PYTHON_BINDINGS_VERSION_MINOR}:")
    LIST(APPEND Boost_LIBRARIES_DEPENDENCIES
            python${ESL_PYTHON_BINDINGS_VERSION_MAJOR}${ESL_PYTHON_BINDINGS_VERSION_MINOR})
    MESSAGE("include: \t" ${PYTHON_INCLUDE_DIRS})
    MESSAGE("library: \t" ${PYTHON_LIBRARIES})
ENDIF()

IF(WITH_MPI)
    LIST(APPEND Boost_LIBRARIES_DEPENDENCIES mpi)
    IF(WITH_PYTHON)
        LIST(APPEND Boost_LIBRARIES_DEPENDENCIES
                mpi_python)#${ESL_PYTHON_BINDINGS_VERSION_MAJOR}${ESL_PYTHON_BINDINGS_VERSION_MINOR})
    ENDIF()
ENDIF()

file(GLOB_RECURSE SOURCE_FILES "esl/**.cpp" "esl/**.hpp")

########################################################################################################################
LIST(APPEND ALL_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR} SYSTEM)


IF(MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS " -static")
    SET(Boost_USE_STATIC_RUNTIME     ON) # not necessary on linux
    SET(Boost_USE_STATIC_LIBS        ON)
ELSE()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON) # MINGW?
ENDIF()

SET(Boost_USE_MULTITHREADED      ON)


IF(MSVC)
    MESSAGE("################################################################################")
    MESSAGE("MSVC build detected, please use NuGet package manager to install dependencies if not yet satisfied: ")
    LIST(APPEND VS_DEPENDENCIES "")

    # this is hardcoded until we can find a way to automate it
    SET(MSVC_GSL_VERSION "2.4.0.8788")
    SET(MSVC_VERSION_PACKAGE_MANAGER "vc142")
    SET(MSVC_BOOST_VERSION "1.72.0")
    SET(ESL_MSVC_ARCHITECTURE "x64")

    MESSAGE("\tInstall-Package gsl-msvc-${ESL_MSVC_ARCHITECTURE} -Version ${MSVC_GSL_VERSION}")
    LIST(APPEND ALL_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/packages/gsl-msvc-${ESL_MSVC_ARCHITECTURE}.${MSVC_GSL_VERSION}/build/native")
    LIST(APPEND VS_DEPENDENCIES "gsl-msvc-${ESL_MSVC_ARCHITECTURE}_${MSVC_GSL_VERSION}")
    LIST(APPEND GSL_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/packages/gsl-msvc-${ESL_MSVC_ARCHITECTURE}.${MSVC_GSL_VERSION}/build/native/lib/${ESL_MSVC_ARCHITECTURE}/Release")

    LIST(APPEND VS_DEPENDENCIES "boost_${MSVC_BOOST_VERSION}")
    LIST(APPEND ALL_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/packages/boost.${MSVC_BOOST_VERSION}.0/lib/native/include")

    # to be compliant, we need to set most of the following variables:
    # Boost_LIBRARIES
    LIST(APPEND Boost_LIBRARIES )

    FOREACH(bll ${Boost_LIBRARIES_DEPENDENCIES})
        MESSAGE("\tInstall-Package boost_${bll}-${MSVC_VERSION_PACKAGE_MANAGER} -Version ${MSVC_BOOST_VERSION}")
        #MESSAGE("\tboost_${bll}_${MSVC_BOOST_VERSION}")
        LIST(APPEND VS_DEPENDENCIES "boost_${bll}-${MSVC_VERSION_PACKAGE_MANAGER}_${MSVC_BOOST_VERSION}")

    ENDFOREACH()

    SET(Boost_FOUND TRUE)
    SET(Boost_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/packages/boost.1.72.0.0/lib/native/include/")
    LIST(APPEND Boost_LIBRARY_DIRS)
    FOREACH(bll ${Boost_LIBRARIES_DEPENDENCIES})
        LIST(APPEND Boost_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/packages/boost_${bll}-${MSVC_VERSION_PACKAGE_MANAGER}.${MSVC_BOOST_VERSION}.0/lib/native/")
        SET("Boost_${bll}_FOUND" TRUE)
    ENDFOREACH()

    MESSAGE("statically linking gsl.lib cblas.lib")
    LIST(APPEND Boost_LIBRARIES gsl.lib cblas.lib)

    FOREACH(bll ${Boost_LIBRARIES_DEPENDENCIES})
        MESSAGE("statically linking libboost_${bll}-${MSVC_VERSION_PACKAGE_MANAGER}-mt-s-${ESL_MSVC_ARCHITECTURE}-1_72.lib")
        LIST(APPEND Boost_LIBRARIES "libboost_${bll}-${MSVC_VERSION_PACKAGE_MANAGER}-mt-s-${ESL_MSVC_ARCHITECTURE}-1_72.lib")
    ENDFOREACH()
ELSE()
    FIND_PACKAGE(Boost 1.65 COMPONENTS ${Boost_LIBRARIES_DEPENDENCIES} REQUIRED)
ENDIF()

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
IF(MSVC)
    # TODO: see GSL linking above
ELSE()
    FIND_PACKAGE(GSL REQUIRED)
ENDIF()

################################################################################
MESSAGE("----------------------------------------")
MESSAGE("profile  " ${CMAKE_BUILD_TYPE})
MESSAGE("compiler " ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION} " (" ${CMAKE_CXX_COMPILER} ")")
MESSAGE("----------------------------------------")


IF(MSVC)
    #TARGET_COMPILE_OPTIONS(${ESL_TARGET_NAME} PRIVATE "/MP")



ELSE()
    ADD_COMPILE_OPTIONS(-m64) # should be the default with modern gccs

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

#ADD_DEFINITIONS(-DADEPT_TRACK_NON_FINITE_GRADIENTS)

# Used to benchmark improvements using automatic differentiation
#ADD_DEFINITIONS(-DADEPT_NO_AUTOMATIC_DIFFERENTIATION)

ENABLE_TESTING()
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
    LIST(APPEND ALL_LINKED_LIBRARIES ${PYTHON_LIBRARIES})
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
    MESSAGE("LINKING WITH")
    MESSAGE(${ALL_LINKED_LIBRARIES})
    LINK_LIBRARIES(${ALL_LINKED_LIBRARIES})
ELSEIF(MINGW)
    LINK_LIBRARIES(${ALL_LINKED_LIBRARIES} -static -static-libgcc -static-libstdc++)
ELSE()
    LINK_LIBRARIES(${ALL_LINKED_LIBRARIES})
ENDIF()

################################################################################

IF(MSVC)
    SET(CMAKE_SUPPRESS_REGENERATION true) # supress ZERO_CHECK target for MSVC
    FOREACH(linkdir ${Boost_LIBRARY_DIRS})
        MESSAGE("adding library directory:" ${linkdir})
    ENDFOREACH()
    ADD_DEFINITIONS(-DBOOST_PYTHON_STATIC_LIB) #
    LIST(APPEND linked_libraries ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES})

#    FOREACH(linked ${linked_libraries})
#        MESSAGE("BOOST LINK LIB:" ${linked})
#    ENDFOREACH()
    LINK_DIRECTORIES(${GSL_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
    LINK_LIBRARIES(                          ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES})

ELSEIF(MINGW)
    LINK_DIRECTORIES(Boost_LIBRARY_DIRS GSL_LIBRARY_DIRS)

    LINK_LIBRARIES(                          ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES} -static -static-libgcc -static-libstdc++)
ELSE()

    LINK_LIBRARIES(                          ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES} dl)
ENDIF()













# This needs to be at this position, because for MSVC we need to issue a LINK_DIRECTORIES call before this to get it right
ADD_LIBRARY(${ESL_TARGET_NAME} SHARED ${SOURCE_FILES} )

SET_TARGET_PROPERTIES(${ESL_TARGET_NAME} PROPERTIES PREFIX "lib")

IF(MSVC)
    # multi-processor compilation
    TARGET_COMPILE_OPTIONS(${ESL_TARGET_NAME} PRIVATE "/MP")

    #
    SET(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    SET_PROPERTY(TARGET ${ESL_TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
ENDIF()



IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_COMPILE_DEFINITIONS(${ESL_TARGET_NAME} PUBLIC ESL_RELEASE=0)
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
ADD_CUSTOM_COMMAND(TARGET ${ESL_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${ESL_TARGET_NAME}>
        ${CMAKE_BINARY_DIR}/test/esl/$<TARGET_FILE_NAME:${ESL_TARGET_NAME}>)

################################################################################

FILE(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test*/test_*.cpp)

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
                MESSAGE("\t MPI PYTHON TEST")
                ADD_TEST(NAME ${test_name}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/python/test/
                        COMMAND ${CMAKE_BINARY_DIR}/python/test/${test_name})
            ELSE()
                MESSAGE("\t MPI TEST")
                ADD_TEST(NAME ${test_name}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
                        COMMAND mpirun -np 2 ${CMAKE_BINARY_DIR}/test/${test_name})
            ENDIF()
        ENDIF()
    ELSEIF(${test_name} MATCHES "^(test_python_)")
        MESSAGE("\t PYTHON TEST")
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

################################################################################

IF(WITH_PYTHON)
    ####################################################################################################################
    # create python modules
    # the requirement a file to be parsed as a python module is that it is a uniquely named .cpp file
    # with python_module as part of the filename
    FILE(GLOB_RECURSE ESL_PYTHON_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} esl/**python_module**.cpp)
    FOREACH (source ${ESL_PYTHON_SOURCE})
        GET_FILENAME_COMPONENT(module_name ${source} NAME_WE)
        GET_FILENAME_COMPONENT(module_path ${source} DIRECTORY)

        GET_FILENAME_COMPONENT(python_module_name ${module_path} NAME)

        MESSAGE("PYTHON MODULE " _${python_module_name} ":\t " ${module_path}/_${python_module_name} "\t" ${source})
        PYTHON_ADD_MODULE(_${python_module_name} ${source} ${SOURCE_FILES})

        ADD_CUSTOM_COMMAND(TARGET _${python_module_name}
                POST_BUILD COMMAND
                ${CMAKE_COMMAND} -E copy_if_different#rename #copy_if_different
                $<TARGET_FILE:_${python_module_name}>
                ${CMAKE_BINARY_DIR}/python/${module_path}/$<TARGET_FILE_NAME:_${python_module_name}>)

        IF(EXISTS ${CMAKE_SOURCE_DIR}/${module_path}/__init__.py)
            FILE(COPY
                    ${CMAKE_SOURCE_DIR}/${module_path}/__init__.py
                    DESTINATION
                    ${CMAKE_BINARY_DIR}/python/${module_path}/
                    )
        ELSE()

        ENDIF()

        #ADD_CUSTOM_COMMAND(TARGET ${python_module_name}_
        #        POST_BUILD
        #        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        #        $<TARGET_FILE:${python_module_name}_>
        #        ${CMAKE_BINARY_DIR}/test/esl/$<TARGET_FILE_NAME:${python_module_name}_>)
    ENDFOREACH ()

    ####################################################################################################################
    # create python tests
#    FILE(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/test_*.py)
#    FOREACH(test_src ${TEST_SRCS})
#        GET_FILENAME_COMPONENT(pytest_name ${test_src} NAME_WE)
#        SET(pytest_name "${pytest_name}_python")
#        MESSAGE("PYTEST " ${pytest_name})
#        FILE(COPY ${test_src} DESTINATION ${CMAKE_BINARY_DIR}/python/test/)
#    ENDFOREACH(test_src)
#
#    ADD_CUSTOM_TARGET(pytest ALL COMMENT "pytest")
#
#    SET_TARGET_PROPERTIES(pytest PROPERTIES
#        RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/python/test/)
#
#    IF(PYTHONINTERP_FOUND)
#
#        ADD_TEST(NAME pytest
#                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/python/test/
#                COMMAND ${PYTHON_EXECUTABLE} -m pytest -r a -v ${CMAKE_BINARY_DIR}/python/test/)
#
#    ENDIF()
ENDIF()

IF(WITH_MPI)
    FILE(COPY hostfile DESTINATION ${CMAKE_BINARY_DIR})
ENDIF()

MESSAGE(${CMAKE_BINARY_DIR})

INSTALL(TARGETS  ${ESL_TARGET_NAME} DESTINATION ${CMAKE_BINARY_DIR})

################################################################################

OPTION(BUILD_DOCUMENTATION "Generate documentation" OFF)

IF(BUILD_DOCUMENTATION)
    FIND_PACKAGE(Doxygen)
    if (DOXYGEN_FOUND)
        SET(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/esl.doxygen)
        SET(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/esl.doxygen)

        CONFIGURE_FILE(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        ADD_CUSTOM_TARGET( documentation ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Documentation using Doxygen"
                VERBATIM )
    else (DOXYGEN_FOUND)
        MESSAGE("Doxygen not found")
    endif (DOXYGEN_FOUND)
ENDIF()
