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
    pyqt_plot.py
    pyqt_filter.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio
    COMPONENT "utils"
)

GR_PYTHON_INSTALL(
    PROGRAMS
    create-gnuradio-out-of-tree-project
    gr_plot_char.py
    gr_plot_const.py
    gr_plot_fft.py
    gr_plot_fft_c.py
    gr_plot_fft_f.py
    gr_plot_psd.py
    gr_plot_psd_c.py
    gr_plot_psd_f.py
    gr_plot_float.py
    gr_plot_int.py
    gr_plot_iq.py
    gr_plot_short.py
    gr_plot_qt.py
    gr_filter_design.py
    DESTINATION ${GR_RUNTIME_DIR}
    COMPONENT "utils"
)
