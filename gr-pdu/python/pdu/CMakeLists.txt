# Copyright 2021 Jacob Gilbert
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

gr_python_install(FILES __init__.py pdu_lambda.py
                  DESTINATION ${GR_PYTHON_DIR}/gnuradio/pdu)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)
    set(GR_TEST_TARGET_DEPS "")
    set(GR_TEST_LIBRARY_DIRS "")
    set(GR_TEST_PYTHON_DIRS ${PROJECT_BINARY_DIR}/gnuradio-runtime/python)
    include(GrTest)
    file(GLOB py_qa_test_files "qa_*.py")
    foreach(py_qa_test_file ${py_qa_test_files})
        get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
        if(CMAKE_CROSSCOMPILING)
            get_filename_component(py_qa_test_file ${py_qa_test_file} NAME)
        endif(CMAKE_CROSSCOMPILING)
        gr_add_test(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${py_qa_test_file})
    endforeach(py_qa_test_file)
endif(ENABLE_TESTING)

add_subdirectory(bindings)
