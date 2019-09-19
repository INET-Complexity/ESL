################################################################################

FILE(READ "esl/version.hpp" version_header)

STRING(REGEX MATCH "[0-9a-zA-Z_]*ESL_VERSION_MAJOR[ \t]*=[ \t]*([0-9]+)"
        _ ${version_header})
SET(version_major ${CMAKE_MATCH_1})

STRING(REGEX MATCH "[0-9a-zA-Z_]*ESL_VERSION_MINOR[ \t]*=[ \t]*([0-9]+)"
        _ ${version_header})
SET(version_minor ${CMAKE_MATCH_1})

STRING(REGEX MATCH "[0-9a-zA-Z_]*ESL_VERSION_REVISION[ \t]*=[ \t]*([0-9]+)"
        _ ${version_header})
SET(version_revision ${CMAKE_MATCH_1})

CMAKE_POLICY(SET CMP0048 NEW)

PROJECT(ESL VERSION ${version_major}.${version_minor}.${version_revision}
        DESCRIPTION "Economic Simulation Library" 
        LANGUAGES CXX)

MESSAGE("ESL Version ${PROJECT_VERSION}")
MESSAGE("installing into ${CMAKE_BINARY_DIR}")

################################################################################
# cmake configuration

CMAKE_MINIMUM_REQUIRED(VERSION 3.9.3)

SET(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")

SET(CMAKE_CXX_STANDARD              17)
SET(CMAKE_CXX_STANDARD_REQUIRED     ON)
SET(CMAKE_CXX_EXTENSIONS            OFF)

IF(NOT ESL_TARGET_NAME)
    SET(ESL_TARGET_NAME     "esl")
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE    Release)
ENDIF()

################################################################################

INCLUDE(cotire)

FIND_PROGRAM(CCACHE_FOUND ccache)
IF(CCACHE_FOUND)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
ENDIF(CCACHE_FOUND)

################################################################################
# Library optional add-ons
################################################################################

OPTION(WITH_MPI "Enable MPI using Boost.MPI" ON)
IF(WITH_MPI)
    MESSAGE("Enabling MPI")
    FIND_PACKAGE(MPI REQUIRED)

    ADD_COMPILE_DEFINITIONS(WITH_MPI)
ENDIF()

OPTION(WITH_PYTHON "Enables export of Python bindings using Boost.Python" ON)
IF(WITH_PYTHON)
    ADD_COMPILE_DEFINITIONS(WITH_PYTHON)
ENDIF()

OPTION(WITH_QL  "Enables quantlib" OFF)
IF(WITH_QL)
    ADD_COMPILE_DEFINITIONS(WITH_QL)
ENDIF()


################################################################################

IF(WITH_PYTHON)
    FIND_PACKAGE(PythonLibs 3)  # for building bindings
    FIND_PACKAGE(PythonInterp 3)# for running tests
ENDIF()

################################################################################
LIST(APPEND Boost_LIBRARIES
            program_options
            date_time 
            serialization 
            unit_test_framework
    )

IF(WITH_PYTHON)
    LIST(APPEND Boost_LIBRARIES python3)
ELSE()
ENDIF()

IF(WITH_MPI)
    LIST(APPEND Boost_LIBRARIES mpi)
    IF(WITH_PYTHON)
        LIST(APPEND Boost_LIBRARIES mpi_python3)
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
    list(APPEND ALL_INCLUDE_DIRECTORIES ${MPI_INCLUDE_PATH})
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
    list(APPEND ALL_LINKED_LIBRARIES ${MPI_CXX_LIBRARIES} )
ENDIF()

IF(WITH_QL)
    list(APPEND ALL_LINKED_LIBRARIES ${QuantLib_LIBRARIES})
ENDIF()

LINK_LIBRARIES(${ALL_LINKED_LIBRARIES} dl)
################################################################################

SET(SOURCE_FILES
        esl/version.hpp
        esl/version.cpp

        esl/simulation/identity.hpp
        esl/simulation/identity.cpp
        esl/simulation/identifiable_as.hpp

        esl/simulation/entity.hpp
        esl/simulation/entity.cpp

        esl/mathematics/interval.cpp
        esl/mathematics/interval.hpp

        esl/agent.cpp
        esl/agent.hpp

        esl/interaction/communicator.cpp
        esl/interaction/communicator.hpp

        esl/interaction/header.cpp
        esl/interaction/header.hpp

        esl/interaction/message.cpp
        esl/interaction/message.hpp

        esl/computation/type_code.hpp
        esl/computation/type_marker.hpp
        )


ADD_LIBRARY(${ESL_TARGET_NAME} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${ESL_TARGET_NAME} PROPERTIES PREFIX "")

TARGET_LINK_LIBRARIES(${ESL_TARGET_NAME} ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES} dl)

LINK_LIBRARIES(                          ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES} dl)

################################################################################
# Copy library into testing folder, so it can be linked against by Python tests
ADD_CUSTOM_COMMAND(TARGET ${ESL_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${ESL_TARGET_NAME}>
        ${CMAKE_BINARY_DIR}/test/esl/$<TARGET_FILE_NAME:${ESL_TARGET_NAME}>)

################################################################################

FILE(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test*/test_*.cpp)
#LIST(FILTER TEST_SRCS INCLUDE REGEX "^test/")

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

################################################################################

IF(WITH_PYTHON)

    # create a python module for all .ccp files
    # TODO: make this a separate set, as some unwanted .cpp files may be swept
    #       up in this
    FILE(GLOB_RECURSE ESL_PYTHON_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} esl/**.cpp)
    FOREACH (source ${ESL_PYTHON_SOURCE})
        GET_FILENAME_COMPONENT(module_name ${source} NAME_WE)
        MESSAGE("PYTHON MODULE " ${module_name} "\t" ${source})
        PYTHON_ADD_MODULE(${module_name} ${source})

        ADD_CUSTOM_COMMAND(TARGET ${module_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${module_name}>
                ${CMAKE_BINARY_DIR}/test/esl/$<TARGET_FILE_NAME:${module_name}>)
    ENDFOREACH ()


    FILE(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/test_*.py)
    FOREACH(test_src ${TEST_SRCS})
        GET_FILENAME_COMPONENT(pytest_name ${test_src} NAME_WE)
        SET(pytest_name "${pytest_name}_python")
        MESSAGE("PYTEST " ${pytest_name})
        FILE(COPY ${test_src} DESTINATION ${CMAKE_BINARY_DIR}/test/)
    ENDFOREACH(test_src)

    ADD_CUSTOM_TARGET(pytest ALL COMMENT "pytest")

    SET_TARGET_PROPERTIES(pytest PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/test/)
        
    IF(PYTHONINTERP_FOUND)
        MESSAGE(${PYTHON_EXECUTABLE})
        ADD_CUSTOM_TARGET(make_module ALL
                  COMMAND ${CMAKE_COMMAND} -E touch test/esl/__init__.py)

        ADD_TEST(NAME pytest
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
                COMMAND ${PYTHON_EXECUTABLE} -m pytest -r a -v ${CMAKE_BINARY_DIR}/test/)
    ENDIF()

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