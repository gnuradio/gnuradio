# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

gr_python_install(
    FILES __init__.py
          matrix_interleaver.py
          parse_file_metadata.py
          sigmf_sink_minimal.py
          stream_to_vector_decimator.py
          msg_pair_to_var.py
          msg_meta_to_pair.py
          var_to_msg.py
          pdu_compatibility.py # REMOVE IN 3.11
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocks)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)

    set(GR_TEST_TARGET_DEPS "")
    set(GR_TEST_LIBRARY_DIRS "")
    set(GR_TEST_PYTHON_DIRS ${PROJECT_BINARY_DIR}/gnuradio-runtime/python)

    include(GrTest)
    file(GLOB py_qa_test_files "qa_*.py")

    if(NOT SNDFILE_FOUND)
        list(REMOVE_ITEM py_qa_test_files ${CMAKE_CURRENT_SOURCE_DIR}/qa_wavfile.py)
    endif()

    foreach(py_qa_test_file ${py_qa_test_files})
        get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
        if(CMAKE_CROSSCOMPILING)
            get_filename_component(py_qa_test_file ${py_qa_test_file} NAME)
        endif(CMAKE_CROSSCOMPILING)
        gr_add_test(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${py_qa_test_file})
    endforeach(py_qa_test_file)

endif(ENABLE_TESTING)

add_subdirectory(bindings)
