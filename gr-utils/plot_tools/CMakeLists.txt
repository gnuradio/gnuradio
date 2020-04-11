# Copyright 2011,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install python files and apps
########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    plot_data.py
    plot_fft_base.py
    plot_psd_base.py
    pyqt_plot.py
    pyqt_filter.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio
)

GR_PYTHON_INSTALL(
    PROGRAMS
    gr_plot
    gr_plot_const
    gr_plot_fft
    gr_plot_psd
    gr_plot_iq
    gr_plot_qt
    DESTINATION ${GR_RUNTIME_DIR}
)
