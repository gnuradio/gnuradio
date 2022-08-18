# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

gr_python_install(FILES api_object.py filter_design.py fir_design.py
                  DESTINATION ${GR_PYTHON_DIR}/gnuradio/filter)
