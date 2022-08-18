# Copyright 212 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup python install
########################################################################
include(GrPython)

gr_python_install(
    FILES __init__.py
          am_demod.py
          fm_demod.py
          fm_emph.py
          nbfm_rx.py
          nbfm_tx.py
          standard_squelch.py
          wfm_rcv.py
          wfm_rcv_fmdet.py
          wfm_rcv_pll.py
          wfm_tx.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/analog)

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
