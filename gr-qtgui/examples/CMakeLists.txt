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

include(GrPython)

GR_PYTHON_INSTALL(PROGRAMS
  pyqt_const_c.py
  pyqt_example_c.py
  pyqt_example_f.py
  pyqt_freq_c.py
  pyqt_freq_f.py
  pyqt_histogram_f.py
  pyqt_time_c.py
  pyqt_time_f.py
  pyqt_time_raster_b.py
  pyqt_time_raster_f.py
  pyqt_waterfall_c.py
  pyqt_waterfall_f.py
  DESTINATION ${GR_PKG_QTGUI_EXAMPLES_DIR}
  COMPONENT "qtgui_python"
)

install(
    FILES
    qtgui_tags_viewing.grc
    qtgui_vector_sink_example.grc
    qtgui_message_inputs.grc
    test_qtgui_msg.grc
    DESTINATION ${GR_PKG_QTGUI_EXAMPLES_DIR}
    COMPONENT "qtgui_python"
)
