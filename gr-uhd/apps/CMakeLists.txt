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
GR_PYTHON_INSTALL(
    FILES
    uhd_siggen_base.py
    uhd_app.py
    msgq_runner.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/uhd
)

GR_PYTHON_INSTALL(
    PROGRAMS
    uhd_fft
    uhd_rx_cfile
    uhd_siggen
    uhd_siggen_gui
    uhd_rx_nogui
    DESTINATION ${GR_RUNTIME_DIR}
)
