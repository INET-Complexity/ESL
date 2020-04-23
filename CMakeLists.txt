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

CMAKE_POLICY(SET CMP0048 NEW)

PROJECT(ESL VERSION ${version_major}.${version_minor}.${version_revision}
        DESCRIPTION "Economic Simulation Library" 
        LANGUAGES CXX)

MESSAGE("ESL Version ${PROJECT_VERSION}")
MESSAGE("building binary in ${CMAKE_BINARY_DIR}")


################################################################################
# cmake configuration

CMAKE_MINIMUM_REQUIRED(VERSION 3.9.3)

SET(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")

SET(CMAKE_CXX_STANDARD              17)
SET(CMAKE_CXX_STANDARD_REQUIRED     ON)
SET(CMAKE_CXX_EXTENSIONS            OFF)

#IF(NOT ESL_TARGET_NAME)
    SET(ESL_TARGET_NAME     "esl")
#ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE("Build type (CMAKE_BUILD_TYPE) not set, defaulting to Release build.")
    SET(CMAKE_BUILD_TYPE    Release)
ENDIF()

ADD_DEFINITIONS(-D_NO_W32_PSEUDO_MODIFIERS )


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

OPTION(WITH_MPI "Enable MPI using Boost.MPI" OFF)
IF(WITH_MPI)
    MESSAGE("Enabling MPI")
    FIND_PACKAGE(MPI REQUIRED)
    ADD_DEFINITIONS(-DWITH_MPI)
ENDIF()

OPTION(WITH_PYTHON "Enables export of Python bindings using Boost.Python" OFF)
IF(WITH_PYTHON)
    ADD_DEFINITIONS(-DWITH_PYTHON)
ENDIF()

OPTION(WITH_QL  "Enables QuantLib" OFF)
IF(WITH_QL)
    ADD_DEFINITIONS(-DWITH_QL)
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
            log
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


################################################################################
#SET(Boost_USE_STATIC_LIBS ON)
LIST(APPEND Boost_LIBRARIES
        program_options
        date_time
        serialization
        unit_test_framework
        thread
        #log
        #log_setup
        )
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
FIND_PACKAGE(Boost 1.65 COMPONENTS ${Boost_LIBRARIES} REQUIRED)
################################################################################

# optional accelerators for scientific computing
#SET(BLA_STATIC ON)
FIND_PACKAGE(BLAS)
FIND_PACKAGE(LAPACK)
# GSL is required for optimisation routines
FIND_PACKAGE(GSL REQUIRED)

################################################################################
MESSAGE("----------------------------------------")
MESSAGE("profile  " ${CMAKE_BUILD_TYPE})
MESSAGE("compiler " ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION} " (" ${CMAKE_CXX_COMPILER} ")")
MESSAGE("----------------------------------------")

SET(CMAKE_CXX_FLAGS "-Wall -Wextra")

SET(CMAKE_C_FLAGS_DEBUG "-g -Og --coverage -fprofile-arcs -ftest-coverage") #  -fanalyzer
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Og --coverage -fprofile-arcs -ftest-coverage")

SET(CMAKE_C_FLAGS_RELEASE "-g -O3")
SET(CMAKE_CXX_FLAGS_RELEASE "-g -O3")





IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
    INCLUDE(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
    ADD_DEFINITIONS(-DESL_RELEASE)
    IF(COMPILER_SUPPORTS_MARCH_NATIVE)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    ENDIF()
ELSE()

ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)




#ADD_DEFINITIONS(-DADEPT_RECORDING_PAUSABLE)
#ADD_DEFINITIONS(-DADEPT_STACK_THREAD_UNSAFE)
#ADD_DEFINITIONS(-DADEPT_TRACK_NON_FINITE_GRADIENTS)

#ADD_DEFINITIONS(-DADEPT_NO_AUTOMATIC_DIFFERENTIATION)



ENABLE_TESTING()
################################################################################
list(APPEND ALL_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR} SYSTEM ${EIGEN3_INCLUDE_DIR})
IF(WITH_MPI)
    list(APPEND ALL_INCLUDE_DIRECTORIES ${MPI_INCLUDE_PATH})
ENDIF()
IF(WITH_PYTHON)
    list(APPEND ALL_INCLUDE_DIRECTORIES ${PYTHON_INCLUDE_DIRS})
ENDIF()

IF(WIN32) # for now, assume all builds on windows are done using MSYS2 64 bit
    LIST(APPEND ALL_INCLUDE_DIRECTORIES "C:/msys64/usr/include")
ENDIF()


INCLUDE_DIRECTORIES(${ALL_INCLUDE_DIRECTORIES})
################################################################################

LIST(APPEND ALL_LINKED_LIBRARIES pthread GSL::gsl)

IF(WIN32)
    #LIST(APPEND ALL_LINKED_LIBRARIES )
    #LINK_DIRECTORIES("C:/msys64/usr/lib")
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

LINK_LIBRARIES(${ALL_LINKED_LIBRARIES})

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

        esl/simulation/world.cpp
        esl/simulation/world.hpp

        esl/simulation/agent_collection.cpp
        esl/simulation/agent_collection.hpp

        esl/simulation/model.cpp
        esl/simulation/model.hpp

        esl/computation/environment.cpp
        esl/computation/environment.hpp

        esl/exception.cpp
        esl/exception.hpp

        esl/data/output_base.cpp
        esl/data/output_base.hpp

        esl/data/output.cpp
        esl/data/output.hpp

        esl/data/producer.cpp
        esl/data/producer.hpp

        esl/law/property.cpp
        esl/law/property.hpp

        esl/law/property_collection.cpp
        esl/law/property_collection.hpp

        esl/geography/iso_3166_1_alpha_2.cpp
        esl/geography/iso_3166_1_alpha_2.hpp

        esl/geography/countries.cpp
        esl/geography/countries.hpp

        esl/economics/iso_4217.cpp
        esl/economics/iso_4217.hpp

        esl/economics/accounting/standard.cpp
        esl/economics/accounting/standard.hpp

        esl/economics/currencies.cpp
        esl/economics/currencies.hpp

        esl/law/jurisdiction.cpp
        esl/law/jurisdiction.hpp

        esl/law/jurisdictions.cpp
        esl/law/jurisdictions.hpp

        esl/economics/asset.cpp
        esl/economics/asset.hpp

        esl/economics/cash.cpp
        esl/economics/cash.hpp

        esl/economics/fungibility.hpp
        esl/economics/tangibility.hpp

        esl/economics/money.cpp
        esl/economics/money.hpp

        esl/economics/owner.cpp
        esl/economics/owner.hpp

        esl/economics/price.cpp
        esl/economics/price.hpp

        esl/geography/location.cpp
        esl/geography/location.hpp

        esl/interaction/transfer.cpp
        esl/interaction/transfer.hpp

        esl/economics/accounting/inventory.cpp
        esl/economics/accounting/inventory.hpp

        esl/law/legal_person.cpp
        esl/law/legal_person.hpp

        esl/law/natural_person.cpp
        esl/law/natural_person.hpp

        esl/law/government.cpp
        esl/law/government.hpp

        esl/law/legal_entity.cpp
        esl/law/legal_entity.hpp

        esl/economics/finance/security.cpp
        esl/economics/finance/security.hpp

        esl/economics/finance/share_class.cpp
        esl/economics/finance/share_class.hpp

        esl/mathematics/rational.cpp
        esl/mathematics/rational.hpp

        esl/computation/distributed/mpi_environment.cpp
        esl/computation/distributed/mpi_environment.hpp

        esl/mathematics/variable.cpp
        esl/mathematics/variable.hpp

        test/shared_property_owner.cpp
        test/shared_property_owner.hpp

        test/shared_properties.cpp
        test/shared_properties.hpp

        esl/economics/markets/indication.cpp
        esl/economics/markets/indication.hpp

        esl/economics/finance/company.cpp
        esl/economics/finance/company.hpp

        esl/economics/markets/market.cpp
        esl/economics/markets/market.hpp

        esl/economics/markets/walras/price_setter.cpp
        esl/economics/markets/walras/price_setter.hpp

        esl/economics/markets/walras/differentiable_order_message.cpp
        esl/economics/markets/walras/differentiable_order_message.hpp

        esl/economics/markets/walras/tatonnement.cpp
        esl/economics/markets/walras/tatonnement.hpp

        esl/economics/finance/shareholder.cpp
        esl/economics/finance/shareholder.hpp

        esl/economics/finance/stock.cpp
        esl/economics/finance/stock.hpp

        esl/economics/finance/dividend.cpp
        esl/economics/finance/dividend.hpp

        esl/simulation/parameter/parametrization.cpp
        esl/simulation/parameter/parametrization.hpp

        esl/economics/finance/loan.cpp
        esl/economics/finance/loan.hpp

        esl/geography/country.cpp
        esl/geography/country.hpp

        esl/economics/currency.cpp
        esl/economics/currency.hpp
        esl/economics/accounting/balance_sheet.cpp 
        esl/economics/accounting/balance_sheet.hpp 
        
        esl/economics/accounting/cash_flow_statement.cpp 
        esl/economics/accounting/cash_flow_statement.hpp 
        
        esl/data/format/delimiter_separated_values.cpp 
        esl/data/format/delimiter_separated_values.hpp 
        
        esl/data/serialization.hpp

        esl/economics/finance/bondholder.cpp
        esl/economics/finance/bondholder.hpp

        esl/data/log.cpp
        esl/data/log.hpp

        esl/economics/markets/order_book/order.cpp
        esl/economics/markets/order_book/order.hpp

        esl/economics/markets/order_book/matching_engine.cpp
        esl/economics/markets/order_book/matching_engine.hpp

        esl/data/log.cpp
        esl/data/log.hpp

        esl/data/stream.cpp
        esl/data/stream.hpp

        esl/data/severity.cpp
        esl/data/severity.hpp
        esl/data/representation.cpp
        esl/data/representation.hpp

        esl/economics/finance/debtor.cpp
        esl/economics/finance/debtor.hpp

        esl/data/null_streambuf.hpp
        esl/data/null_ostream.hpp
        esl/data/channel.cpp
        esl/data/channel.hpp

        #esl/economics/markets/order_book/book.cpp
        #esl/economics/markets/order_book/book.hpp

        esl/economics/money.cpp
        esl/economics/finance/securities_lending_contract.cpp
        esl/economics/finance/securities_lending_contract.hpp
        esl/data/format/numpy.cpp esl/data/format/numpy.hpp esl/data/format/xml.cpp esl/data/format/xml.hpp)

ADD_LIBRARY(${ESL_TARGET_NAME} SHARED ${SOURCE_FILES} esl/data/format/numpy.cpp esl/data/format/numpy.hpp)
SET_TARGET_PROPERTIES(${ESL_TARGET_NAME} PROPERTIES PREFIX "lib")


if(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_COMPILE_DEFINITIONS(${ESL_TARGET_NAME} PUBLIC ESL_RELEASE=0)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_COMPILE_DEFINITIONS(${ESL_TARGET_NAME} PUBLIC ESL_RELEASE=1)
endif()


IF(WIN32)

    LINK_DIRECTORIES("C:/msys64/usr/lib")
    TARGET_LINK_LIBRARIES(${ESL_TARGET_NAME} ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES})
    LINK_LIBRARIES(                          ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES})
ELSE()
    TARGET_LINK_LIBRARIES(${ESL_TARGET_NAME} ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES} dl)
    LINK_LIBRARIES(                          ${Boost_LIBRARIES} ${ALL_LINKED_LIBRARIES} dl)
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
    MESSAGE("TEST " ${test_src})
    GET_FILENAME_COMPONENT(test_name ${test_src} NAME_WE)

    ADD_EXECUTABLE(${test_name} ${test_src} ${SOURCE_FILES})
    TARGET_LINK_LIBRARIES(${test_name} ${Boost_LIBRARIES} ${ESL_TARGET_NAME})
    SET_TARGET_PROPERTIES(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/test/)

    # test_mpi_* and test_mpi_python_* need to be run using a MPI job runner
    IF(${test_name} MATCHES "^(test_mpi_)")
        IF(WITH_MPI)
            MESSAGE("\t MPI TEST")
            ADD_TEST(NAME ${test_name}
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
                    COMMAND mpirun -np 2 ${CMAKE_BINARY_DIR}/test/${test_name})
        ENDIF()
    ELSE()
        ADD_TEST(NAME ${test_name}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
                COMMAND ${CMAKE_BINARY_DIR}/test/${test_name})
    ENDIF()
ENDFOREACH(test_src)

################################################################################

IF(WITH_PYTHON)
    # create a python module for all .ccp files
    # TODO: make this a separate set, as some unwanted .cpp files may be swept
    #       up in this
    FILE(GLOB_RECURSE ESL_PYTHON_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} esl/**.cpp)
    FOREACH (source ${ESL_PYTHON_SOURCE})
        GET_FILENAME_COMPONENT(module_name ${source} NAME_WE)
        GET_FILENAME_COMPONENT(module_path ${source} DIRECTORY)

        MESSAGE("PYTHON MODULE " ${module_path}/${module_name} "\t" ${source})
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
              COMMAND ${CMAKE_COMMAND} -E make_directory test/esl/ && ${CMAKE_COMMAND} -E touch ./test/esl/__init__.py)

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
