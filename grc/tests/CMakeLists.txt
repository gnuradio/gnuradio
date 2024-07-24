# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

if(ENABLE_TESTING)
    GR_PYTHON_CHECK_MODULE_RAW(
        "pytest"
        "import pytest"
        PYTEST_FOUND
    )

    if(PYTEST_FOUND)
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
