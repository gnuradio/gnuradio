# Copyright 2011-212 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup python install
########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    bpsk.py
    cpm.py
    crc.py
    generic_mod_demod.py
    gmsk.py
    gfsk.py
    modulation_utils.py
    ofdm.py
    ofdm_packet_utils.py
    ofdm_receiver.py
    ofdm_sync_fixed.py
    ofdm_sync_ml.py
    ofdm_sync_pnac.py
    ofdm_sync_pn.py
    ofdm_txrx.py
    packet_utils.py
    pkt.py
    psk.py
    qam.py
    qamlike.py
    qpsk.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/digital
    COMPONENT "digital_python"
)

GR_PYTHON_INSTALL(
    FILES
    utils/__init__.py
    utils/gray_code.py
    utils/mod_codes.py
    utils/alignment.py
    utils/tagged_streams.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/digital/utils
    COMPONENT "digital_python"
)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)

list(APPEND GR_TEST_PYTHON_DIRS
    ${CMAKE_BINARY_DIR}/gr-digital/python
    ${CMAKE_BINARY_DIR}/gr-digital/swig
    ${CMAKE_BINARY_DIR}/gr-filter/python
    ${CMAKE_BINARY_DIR}/gr-filter/swig
    ${CMAKE_BINARY_DIR}/gr-analog/python
    ${CMAKE_BINARY_DIR}/gr-analog/swig
    ${CMAKE_BINARY_DIR}/gr-blocks/python
    ${CMAKE_BINARY_DIR}/gr-blocks/swig
    ${CMAKE_BINARY_DIR}/gr-fft/python
    ${CMAKE_BINARY_DIR}/gr-fft/swig
    ${CMAKE_BINARY_DIR}/gr-channels/python
    ${CMAKE_BINARY_DIR}/gr-channels/swig
)
list(APPEND GR_TEST_TARGET_DEPS gnuradio-digital gnuradio-filter gnuradio-fft gnuradio-analog gnuradio-blocks)

include(GrTest)
file(GLOB py_qa_test_files "qa_*.py")
foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    GR_ADD_TEST(${py_qa_test_name} ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B} ${py_qa_test_file})
endforeach(py_qa_test_file)
endif(ENABLE_TESTING)
