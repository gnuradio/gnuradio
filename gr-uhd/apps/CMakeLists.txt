# Copyright 2011,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

########################################################################
# Install some uhd apps
########################################################################
gr_python_install(FILES uhd_siggen_base.py uhd_app.py msgq_runner.py
                  DESTINATION ${GR_PYTHON_DIR}/gnuradio/uhd)


configure_file(uhd_fft ${CMAKE_BINARY_DIR}/uhd_fft @ONLY)
configure_file(uhd_rx_cfile ${CMAKE_BINARY_DIR}/uhd_rx_cfile @ONLY)
configure_file(uhd_siggen ${CMAKE_BINARY_DIR}/uhd_siggen @ONLY)
configure_file(uhd_siggen_gui ${CMAKE_BINARY_DIR}/uhd_siggen_gui @ONLY)
configure_file(uhd_rx_nogui ${CMAKE_BINARY_DIR}/uhd_rx_nogui @ONLY)

gr_python_install(PROGRAMS
                    ${CMAKE_BINARY_DIR}/uhd_fft
                    ${CMAKE_BINARY_DIR}/uhd_rx_cfile
                    ${CMAKE_BINARY_DIR}/uhd_siggen
                    ${CMAKE_BINARY_DIR}/uhd_siggen_gui
                    ${CMAKE_BINARY_DIR}/uhd_rx_nogui
                  DESTINATION ${GR_RUNTIME_DIR})

gen_py_launcher(
    MODULES uhd_fft uhd_rx_cfile uhd_siggen uhd_siggen_gui uhd_rx_nogui
    PYTHON ${GR_BUNDLE_PYTHON}
)