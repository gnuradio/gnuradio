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

configure_file(gr_plot ${CMAKE_BINARY_DIR}/gr_plot @ONLY)
configure_file(gr_plot_const ${CMAKE_BINARY_DIR}/gr_plot_const @ONLY)
configure_file(gr_plot_fft ${CMAKE_BINARY_DIR}/gr_plot_fft @ONLY)
configure_file(gr_plot_psd ${CMAKE_BINARY_DIR}/gr_plot_psd @ONLY)
configure_file(gr_plot_iq ${CMAKE_BINARY_DIR}/gr_plot_iq @ONLY)
configure_file(gr_plot_qt ${CMAKE_BINARY_DIR}/gr_plot_qt @ONLY)
if(WIN32)
gen_py_launcher(
    MODULES gr_plot gr_plot_const gr_plot_fft gr_plot_psd gr_plot_iq gr_plot_qt
    PYTHON ${GR_BUNDLE_PYTHON}
)
elseif(APPLE)
gen_py_program_launcher(
    PROGRAMS gr_plot gr_plot_const gr_plot_fft gr_plot_psd gr_plot_iq gr_plot_qt
)
endif()
GR_PYTHON_INSTALL(
    PROGRAMS
    ${CMAKE_BINARY_DIR}/gr_plot
    ${CMAKE_BINARY_DIR}/gr_plot_const
    ${CMAKE_BINARY_DIR}/gr_plot_fft
    ${CMAKE_BINARY_DIR}/gr_plot_psd
    ${CMAKE_BINARY_DIR}/gr_plot_iq
    ${CMAKE_BINARY_DIR}/gr_plot_qt
    DESTINATION ${GR_RUNTIME_DIR}
)
