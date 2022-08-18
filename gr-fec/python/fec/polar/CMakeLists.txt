# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

gr_python_install(
    FILES __init__.py
          channel_construction.py
          channel_construction_awgn.py
          channel_construction_bec.py
          helper_functions.py
          encoder.py
          decoder.py
          common.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fec/polar)

gr_python_install(PROGRAMS polar_channel_construction DESTINATION ${GR_RUNTIME_DIR})
