# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

gr_python_install(
    PROGRAMS affinity_set.py plot_flops.py run_synthetic.py synthetic.py
             wfm_rcv_pll_to_wav.py DESTINATION ${GR_PKG_DATA_DIR}/examples/mp-sched)

install(FILES README DESTINATION ${GR_PKG_DATA_DIR}/examples/mp-sched)
