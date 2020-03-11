# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

GR_PYTHON_INSTALL(
    PROGRAMS
    freq_hopping.py
    DESTINATION ${GR_PKG_UHD_EXAMPLES_DIR}
)
