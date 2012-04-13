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

include_directories(${GR_AUDIO_INCLUDE_DIRS})
include_directories(${GNURADIO_CORE_INCLUDE_DIRS})
add_executable(dial_tone dial_tone.cc)
target_link_libraries(dial_tone gnuradio-audio)

INSTALL(TARGETS
    dial_tone
    DESTINATION ${GR_PKG_AUDIO_EXAMPLES_DIR}
    COMPONENT "audio_examples"
)
