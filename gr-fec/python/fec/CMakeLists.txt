# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    bitflip.py
    extended_encoder.py
    extended_decoder.py
    capillary_threaded_decoder.py
    capillary_threaded_encoder.py
    threaded_decoder.py
    threaded_encoder.py
    extended_async_encoder.py
    extended_tagged_encoder.py
    extended_tagged_decoder.py
    fec_test.py
    bercurve_generator.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fec
)

add_subdirectory(polar)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)

list(APPEND GR_TEST_PYTHON_DIRS
    ${PROJECT_BINARY_DIR}/gnuradio-runtime/python
    ${PROJECT_BINARY_DIR}/gr-fec/python
    ${PROJECT_BINARY_DIR}/gr-blocks/python
)

list(APPEND GR_TEST_TARGET_DEPS gnuradio-fec)

include(GrTest)
file(GLOB py_qa_test_files "qa_*.py")

# Without GSL, we don't build some of the LDPC work, so we can't test
# it here.
if(NOT GSL_FOUND)
  list(REMOVE_ITEM py_qa_test_files
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_fecapi_ldpc.py
    )
endif(NOT GSL_FOUND)

foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    if (CMAKE_CROSSCOMPILING)
        get_filename_component(py_qa_test_file ${py_qa_test_file} NAME)
    endif(CMAKE_CROSSCOMPILING)
    GR_ADD_TEST(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${py_qa_test_file})
endforeach(py_qa_test_file)

endif(ENABLE_TESTING)

add_subdirectory(bindings)
