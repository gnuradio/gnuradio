# Copyright 2011,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

add_executable(dial_tone dial_tone.cc)
target_link_libraries(dial_tone gnuradio-runtime gnuradio-audio gnuradio-analog)

install(TARGETS dial_tone DESTINATION ${GR_PKG_AUDIO_EXAMPLES_DIR})
