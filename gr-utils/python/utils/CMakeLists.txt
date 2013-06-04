# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

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
    COMPONENT "utils"
)

GR_PYTHON_INSTALL(
    PROGRAMS
    gr_modtool
    gr_plot_char
    gr_plot_const
    gr_plot_fft
    gr_plot_fft_c
    gr_plot_fft_f
    gr_plot_psd
    gr_plot_psd_c
    gr_plot_psd_f
    gr_plot_float
    gr_plot_int
    gr_plot_iq
    gr_plot_short
    gr_plot_qt
    gr_read_file_metadata
    grcc
    DESTINATION ${GR_RUNTIME_DIR}
    COMPONENT "utils"
)
