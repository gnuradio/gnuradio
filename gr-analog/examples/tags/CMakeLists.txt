# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

GR_PYTHON_INSTALL(PROGRAMS
  uhd_burst_detector.py
  DESTINATION ${GR_PKG_DATA_DIR}/examples/tags
)
