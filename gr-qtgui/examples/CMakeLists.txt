# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

gr_python_install(
    PROGRAMS pyqt_const_c.py
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
    DESTINATION ${GR_PKG_QTGUI_EXAMPLES_DIR})

install(
    FILES qtgui_message_inputs.grc
          qtgui_multi_input.grc
          qtgui_tags_viewing.grc
          qtgui_vector_sink_example.grc
          test_qtgui_msg.grc
          test_autocorrelator.grc
          test_appbackground.grc
          show_controls2.grc
          test_toggleswitch.grc
          test_toggleswitch2.grc
          test_toggle.grc
          test_msgpush.grc
          test_msgcheckbox.grc
          test_levelgauge.grc
          test_led.grc
          test_graphicscaled.grc
          test_graphicitem.grc
          test_distanceradar.grc
          test_digitalnumcontrol_ro.grc
          test_digitalnumcontrol.grc
          test_dialcontrol.grc
          test_dialcontrol2.grc
          test_compass_stream.grc
          test_compass.grc
          show_dialgauge2.grc
          show_dialgauge1.grc
          show_controls.grc
          test_msgcheckbox_str.grc
          test_graphicitem_overlay.grc
          gnuradio_logo.png
          satdishsmall.png
          earth.jpg
    DESTINATION ${GR_PKG_QTGUI_EXAMPLES_DIR})
