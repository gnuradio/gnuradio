# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

gr_python_install(PROGRAMS uhd_burst_detector.py
                  DESTINATION ${GR_PKG_DATA_DIR}/examples/tags)
