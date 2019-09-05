################################################################################

file(READ "esl/version.hpp" version_header)

string(REGEX MATCH "[0-9a-zA-Z_]*ESL_VERSION_MAJOR[ \t]*=[ \t]*([0-9]+)" 
        _ ${version_header})
set(version_major ${CMAKE_MATCH_1})

string(REGEX MATCH "[0-9a-zA-Z_]*ESL_VERSION_MINOR[ \t]*=[ \t]*([0-9]+)" 
        _ ${version_header})
set(version_minor ${CMAKE_MATCH_1})

string(REGEX MATCH "[0-9a-zA-Z_]*ESL_VERSION_REVISION[ \t]*=[ \t]*([0-9]+)" 
        _ ${version_header})
set(version_revision ${CMAKE_MATCH_1})

cmake_policy(SET CMP0048 NEW)

project(ESL VERSION ${version_major}.${version_minor}.${version_revision}
        DESCRIPTION "Economic Simulation Library" 
        LANGUAGES CXX)

MESSAGE("ESL Version ${PROJECT_VERSION}")
MESSAGE("installing into ${CMAKE_BINARY_DIR}")

################################################################################
# cmake configuration

cmake_minimum_required(VERSION 3.9.3)

set(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)

IF(NOT ESL_TARGET_NAME)
    SET(ESL_TARGET_NAME     "esl")
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE    Release)
ENDIF()

################################################################################

include(cotire)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

################################################################################
# Library optional add-ons

option(WITH_MPI "Enable MPI using Boost.MPI" OFF)
IF(WITH_MPI)
    MESSAGE("Enabling MPI")
    find_package(MPI REQUIRED)

    ADD_COMPILE_DEFINITIONS(WITH_MPI)
ENDIF()

option(WITH_PYTHON "Enables export of Python bindings using Boost.Python" ON)
IF(WITH_PYTHON)
    ADD_COMPILE_DEFINITIONS(WITH_PYTHON)
ENDIF()

option(WITH_QL  "Enables quantlib" OFF)
IF(WITH_QL)
    ADD_COMPILE_DEFINITIONS(WITH_QL)
ENDIF()


################################################################################

IF(WITH_PYTHON)
    FIND_PACKAGE(PythonLibs)                     # for building bindings
    FIND_PACKAGE(Python3 COMPONENTS Interpreter) # for running tests
ENDIF()

################################################################################
list(APPEND Boost_LIBRARIES 
            program_options
            date_time 
            serialization 
            unit_test_framework
    )

IF(WITH_PYTHON)
    list(APPEND Boost_LIBRARIES python3)
ELSE()
ENDIF()

IF(WITH_MPI)
    list(APPEND Boost_LIBRARIES mpi)
    IF(WITH_PYTHON)
        list(APPEND Boost_LIBRARIES mpi_python3)
    ENDIF()
ENDIF()

FIND_PACKAGE(Boost 1.65 COMPONENTS ${Boost_LIBRARIES} REQUIRED)

FIND_PACKAGE(Eigen3 3.3 REQUIRED NO_MODULE)
################################################################################
MESSAGE("----------------------------------------")
MESSAGE("profile  " ${CMAKE_BUILD_TYPE})
MESSAGE("compiler " ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION} " (" ${CMAKE_CXX_COMPILER} ")")
MESSAGE("----------------------------------------")

SET(CMAKE_CXX_FLAGS "-Wall -Wextra")

SET(CMAKE_C_FLAGS_DEBUG "-g -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -ftime-report")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -ftime-report")

SET(CMAKE_C_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

ENABLE_TESTING()
################################################################################
list(APPEND ALL_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR} SYSTEM "/usr/local/include/eigen3/")
IF(WITH_MPI)
    list(APPEND ALL_INCLUDE_DIRECTORIES MPI_INCLUDE_PATH )
ENDIF()
IF(WITH_PYTHON)
    list(APPEND ALL_INCLUDE_DIRECTORIES ${PYTHON_INCLUDE_DIRS})
ENDIF()
INCLUDE_DIRECTORIES(${ALL_INCLUDE_DIRECTORIES})
################################################################################
list(APPEND ALL_LINKED_LIBRARIES pthread dl util)

IF(WITH_PYTHON)
    list(APPEND ALL_LINKED_LIBRARIES ${PYTHON_LIBRARIES})
ENDIF()

IF(WITH_MPI)
    list(APPEND ALL_LINKED_LIBRARIES ${MPI_LIBRARIES})
ENDIF()

IF(WITH_QL)
    list(APPEND ALL_LINKED_LIBRARIES ${QuantLib_LIBRARIES})
ENDIF()

LINK_LIBRARIES(${ALL_LINKED_LIBRARIES} dl)
################################################################################




SET(SOURCE_FILES
        esl/version.hpp
        esl/version.cpp
)


ADD_LIBRARY(${ESL_TARGET_NAME} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${ESL_TARGET_NAME} PROPERTIES PREFIX "")

TARGET_LINK_LIBRARIES(${ESL_TARGET_NAME} ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES} dl)

LINK_LIBRARIES(                          ${Boost_LIBRARIES} ${ESL_TARGET_NAME})



ADD_CUSTOM_COMMAND(TARGET ${ESL_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${ESL_TARGET_NAME}>
        ${CMAKE_BINARY_DIR}/test/$<TARGET_FILE_NAME:${ESL_TARGET_NAME}>)




SET(TEST_SOURCE_FILES
        test/test_version.cpp
        )

FILE(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/test_*.cpp)
FOREACH(test_src ${TEST_SRCS})
    MESSAGE("TEST " ${test_src})
    GET_FILENAME_COMPONENT(test_name ${test_src} NAME_WE)

    ADD_EXECUTABLE(${test_name} ${test_src})
    TARGET_LINK_LIBRARIES(${test_name} ${Boost_LIBRARIES} ${ESL_TARGET_NAME})
    SET_TARGET_PROPERTIES(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/test/)

    ADD_TEST(NAME ${test_name}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
            COMMAND ${CMAKE_BINARY_DIR}/test/${test_name})
ENDFOREACH(test_src)


IF(WITH_PYTHON)
    PYTHON_ADD_MODULE(version esl/version.cpp)

    FILE(COPY test/test_version.py DESTINATION ${CMAKE_BINARY_DIR}/test/)

    FILE(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/test_*.py)
    FOREACH(test_src ${TEST_SRCS})
        GET_FILENAME_COMPONENT(pytest_name ${test_src} NAME_WE)
        SET(pytest_name "${pytest_name}_python")
        MESSAGE("PYTEST " ${pytest_name})
    ENDFOREACH(test_src)


    ADD_CUSTOM_TARGET(pytest ALL COMMENT "pytest")

    SET_TARGET_PROPERTIES(pytest PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/test/)

    IF(Python3_Interpreter_FOUND)
        ADD_CUSTOM_TARGET(make_module ALL
                  COMMAND ${CMAKE_COMMAND} -E touch test/__init__.py)


        ADD_TEST(NAME pytest
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
                COMMAND ${Python3_EXECUTABLE} -m pytest -r a -v ${CMAKE_BINARY_DIR}/test/)
    ENDIF()

ENDIF()

IF(WITH_MPI)
    FILE(COPY hostfile DESTINATION ${CMAKE_BINARY_DIR}/cmake-build/)
ENDIF()

MESSAGE(${CMAKE_BINARY_DIR})





INSTALL(TARGETS  ${ESL_TARGET_NAME} DESTINATION ${CMAKE_BINARY_DIR})