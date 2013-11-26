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
include(GrPython)

########################################################################
# Install python files into wxgui module
########################################################################
GR_PYTHON_INSTALL(
    FILES
    __init__.py
    common.py
    constants.py
    constsink_gl.py
    const_window.py
    form.py
    fftsink2.py
    fftsink_nongl.py
    fftsink_gl.py
    fft_window.py
    gui.py
    histosink_gl.py
    histo_window.py
    numbersink2.py
    number_window.py
    plot.py
    powermate.py
    pubsub.py
    scopesink2.py
    scopesink_nongl.py
    scopesink_gl.py
    scope_window.py
    termsink.py
    waterfallsink2.py
    waterfallsink_nongl.py
    waterfallsink_gl.py
    waterfall_window.py
    slider.py
    stdgui2.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/wxgui
    COMPONENT "wxgui_python"
)

########################################################################
# Install python files into wxgui forms sub-module
########################################################################
GR_PYTHON_INSTALL(
    FILES
    forms/__init__.py
    forms/forms.py
    forms/converters.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/wxgui/forms
    COMPONENT "wxgui_python"
)

########################################################################
# Install python files into wxgui plotter sub-module
########################################################################
GR_PYTHON_INSTALL(
    FILES
    plotter/__init__.py
    plotter/bar_plotter.py
    plotter/channel_plotter.py
    plotter/common.py
    plotter/gltext.py
    plotter/grid_plotter_base.py
    plotter/plotter_base.py
    plotter/waterfall_plotter.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/wxgui/plotter
    COMPONENT "wxgui_python"
)
