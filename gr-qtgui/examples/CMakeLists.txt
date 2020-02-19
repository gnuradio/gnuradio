# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

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
)

install(
    FILES
    qtgui_message_inputs.grc
    qtgui_multi_input.grc
    qtgui_tags_viewing.grc
    qtgui_vector_sink_example.grc
    test_qtgui_msg.grc
    DESTINATION ${GR_PKG_QTGUI_EXAMPLES_DIR}
)
