# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

if(ENABLE_TESTING)
    gr_python_check_module_raw("pytest" "import pytest" PYTEST_FOUND)

    if(PYTEST_FOUND)
        set(GR_TEST_ENVIRONS "GRC_BLOCKS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../blocks\"")
        gr_add_test(
            grc_tests
            ${QA_PYTHON_EXECUTABLE}
            -B
            -m
            pytest
            ${CMAKE_CURRENT_SOURCE_DIR}
            -m
            \"not
            examples\")
    endif()
endif(ENABLE_TESTING)
