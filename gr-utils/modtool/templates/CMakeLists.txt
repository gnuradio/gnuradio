# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

gr_python_install(FILES __init__.py templates.py
                  DESTINATION ${GR_PYTHON_DIR}/gnuradio/modtool/templates)

set(GR_PKG_MODTOOL_DATA_DIR ${GR_PKG_DATA_DIR}/modtool/templates)
install(DIRECTORY gr-newmod DESTINATION ${GR_PKG_MODTOOL_DATA_DIR})
