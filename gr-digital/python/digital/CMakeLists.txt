# Copyright 2011-2013 Free Software Foundation, Inc.
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
          bpsk.py
          constellation_map_generator.py
          cpm.py
          generic_mod_demod.py
          gmsk.py
          gfsk.py
          modulation_utils.py
          ofdm_txrx.py
          packet_utils.py
          psk.py
          psk_constellations.py
          qam.py
          qamlike.py
          qam_constellations.py
          qpsk.py
          soft_dec_lut_gen.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/digital)

gr_python_install(
    FILES utils/__init__.py utils/gray_code.py utils/lfsr.py utils/mod_codes.py
          utils/alignment.py utils/tagged_streams.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/digital/utils)

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
