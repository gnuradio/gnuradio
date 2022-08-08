# Copyright 2011-212 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup python install
########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/uhd
)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)

list(APPEND GR_TEST_PYTHON_DIRS
    ${PROJECT_BINARY_DIR}/gr-uhd/python
    ${PROJECT_BINARY_DIR}/gnuradio-runtime/python
)

list(APPEND GR_TEST_TARGET_DEPS gnuradio-uhd)

include(GrTest)
file(GLOB py_qa_test_files "qa_*.py")
foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    if (CMAKE_CROSSCOMPILING)
        get_filename_component(py_qa_test_file ${py_qa_test_file} NAME)
    endif(CMAKE_CROSSCOMPILING)
    GR_ADD_TEST(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${py_qa_test_file})
endforeach(py_qa_test_file)
endif(ENABLE_TESTING)

add_subdirectory(bindings)
