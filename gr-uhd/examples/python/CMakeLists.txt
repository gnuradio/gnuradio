# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

gr_python_install(PROGRAMS freq_hopping.py DESTINATION ${GR_PKG_UHD_EXAMPLES_DIR})
