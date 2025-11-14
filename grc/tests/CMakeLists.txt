# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

if(ENABLE_TESTING)
    GR_PYTHON_CHECK_MODULE(
        DESC "pytest"
        MODULE pytest
        VAR PYTEST_FOUND
    )

    if(PYTEST_FOUND)
        # Register pytest component and dependencies for 
        # installer packaging as defined in METADATA file of pytest
        REGISTER_EXTERNAL_PYTHON_COMPONENT(iniconfig)
        REGISTER_EXTERNAL_PYTHON_COMPONENT(packaging)
        REGISTER_EXTERNAL_PYTHON_COMPONENT(pluggy)
        if(${PYTHON_VERSION_MAJOR} LESS 3 OR (${PYTHON_VERSION_MAJOR} EQUAL 3 AND ${PYTHON_VERSION_MINOR} LESS 11))
            # Python version is less than 3.11, see METADATA
            REGISTER_EXTERNAL_PYTHON_COMPONENT(exceptiongroup)
            REGISTER_EXTERNAL_PYTHON_COMPONENT(tomli)
        endif()
        if (WIN32)
            REGISTER_EXTERNAL_PYTHON_COMPONENT(colorama)
        endif()
        # the following packages are listed in METADATA of pytest, 
        # but they are not required for pure pytest installation:
        #REGISTER_EXTERNAL_PYTHON_COMPONENT(argcomplete)
        #REGISTER_EXTERNAL_PYTHON_COMPONENT(hypothesis)
        #REGISTER_EXTERNAL_PYTHON_COMPONENT(mock)
        #REGISTER_EXTERNAL_PYTHON_COMPONENT(xmlschema)
        # the following packages are listed in METADATA of pytest, 
        # and required for pure pytest installation:
        REGISTER_EXTERNAL_PYTHON_COMPONENT(attrs)
        REGISTER_EXTERNAL_PYTHON_COMPONENT(pygments)
        REGISTER_EXTERNAL_PYTHON_COMPONENT(requests)
        REGISTER_EXTERNAL_PYTHON_COMPONENT(setuptools)
        # these packages are not listed in METADATA of pytest, 
        # but needed for a successful pytest installation:
        REGISTER_EXTERNAL_PYTHON_COMPONENT(py)
        REGISTER_EXTERNAL_PYTHON_COMPONENT(_pytest)
        REGISTER_EXTERNAL_PYTHON_COMPONENT(pytest)
        message(STATUS "pytest dependencies registered for installer packaging")

        execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/state)
        set(GR_TEST_ENVIRONS
            "GRC_PREFS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\""
            "GRC_BLOCKS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../blocks\""
            "GRC_HIER_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/state\"")
        GR_ADD_TEST(grc_tests ${QA_PYTHON_EXECUTABLE} -B -m pytest --ignore ${CMAKE_CURRENT_SOURCE_DIR}/test_qtbot.py ${CMAKE_CURRENT_SOURCE_DIR} -m \"not examples\" --log-cli-level=DEBUG)
        # To run the grcc tests over examples manually, run
        # python3 -B -m pytest gnuradio/grc/tests -m "examples"
    endif()
endif(ENABLE_TESTING)
