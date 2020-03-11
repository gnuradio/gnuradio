# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

GR_PYTHON_INSTALL(PROGRAMS
  volk_math.py
  volk_plot.py
  volk_test_funcs.py
  volk_types.py
  DESTINATION ${GR_PKG_DATA_DIR}/examples/volk_benchmark
)

install(
    FILES README
    DESTINATION ${GR_PKG_DATA_DIR}/examples/volk_benchmark
)
