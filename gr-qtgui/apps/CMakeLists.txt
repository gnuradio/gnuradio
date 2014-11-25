# Copyright 2012 Free Software Foundation, Inc.
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

include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    plot_base.py
    plot_psd_base.py
    plot_spectrogram_base.py
    plot_time_base.py
    plot_time_raster_base.py
    plot_form.py
    plot_constellation_form.py
    plot_psd_form.py
    plot_spectrogram_form.py
    plot_time_form.py
    plot_time_raster_form.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/qtgui
    COMPONENT "qtgui_python"
)

GR_PYTHON_INSTALL(
    PROGRAMS
    gr_time_plot_c
    gr_time_plot_f
    gr_time_plot_i
    gr_time_plot_s
    gr_time_plot_b
    gr_time_raster_f
    gr_time_raster_b
    gr_psd_plot_c
    gr_psd_plot_f
    gr_psd_plot_i
    gr_psd_plot_s
    gr_psd_plot_b
    gr_spectrogram_plot_c
    gr_spectrogram_plot_f
    gr_spectrogram_plot_i
    gr_spectrogram_plot_s
    gr_spectrogram_plot_b
    gr_spectrogram_plot
    gr_constellation_plot
    DESTINATION ${GR_RUNTIME_DIR}
    COMPONENT "qtgui_python"
)
