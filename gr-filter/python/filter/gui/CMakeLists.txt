# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

gr_python_install(
    FILES bandgraphicsview.py
          banditems.py
          icons_rc.py
          idealbanditems.py
          polezero_plot.py
          pyqt_filter_stacked.py
          CustomViewBox.py
          GrFilterPlotWidget.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/filter)
