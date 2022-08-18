# Copyright 2016,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include_directories(
    ${GR_QTGUI_INCLUDE_DIRS}
    ${GR_ANALOG_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GR_FFT_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${QT_INCLUDE_DIRS})

list(
    APPEND
    QTGUI_LIBRARIES
    gnuradio-qtgui
    gnuradio-analog
    gnuradio-filter
    gnuradio-blocks
    gnuradio-fft
    gnuradio-runtime
    ${QT_LIBRARIES})

qt5_wrap_cpp(qtgui_moc_sources display_qt.h)

add_executable(display_qt display_qt.cc ${qtgui_moc_sources})
target_link_libraries(display_qt ${QTGUI_LIBRARIES})

install(TARGETS display_qt DESTINATION ${GR_PKG_QTGUI_EXAMPLES_DIR})
