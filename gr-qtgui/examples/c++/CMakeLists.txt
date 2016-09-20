# Copyright 2016 Free Software Foundation, Inc.
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

include_directories(
  ${GR_QTGUI_INCLUDE_DIRS}
  ${GR_ANALOG_INCLUDE_DIRS}
  ${GR_FILTER_INCLUDE_DIRS}
  ${GR_BLOCKS_INCLUDE_DIRS}
  ${GR_FFT_INCLUDE_DIRS}
  ${GNURADIO_RUNTIME_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

list(APPEND QTGUI_LIBRARIES
  gnuradio-qtgui
  gnuradio-analog
  gnuradio-filter
  gnuradio-blocks
  gnuradio-fft
  gnuradio-runtime
)

QT4_WRAP_CPP(qtgui_moc_sources display_qt.h)
add_executable(display_qt display_qt.cc ${qtgui_moc_sources})
target_link_libraries(display_qt ${QTGUI_LIBRARIES})

INSTALL(TARGETS
  display_qt
  DESTINATION ${GR_PKG_QTGUI_EXAMPLES_DIR}
  COMPONENT "qtgui_examples"
)
