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

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GR_ANALOG_INCLUDE_DIRS}
    ${GR_CHANNELS_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND channels_sources
  channel_model_impl.cc
)

list(APPEND channels_libs
    volk
    gnuradio-core
    gnuradio-filter
    gnuradio-analog
    gnuradio-blocks
    ${Boost_LIBRARIES}
)

add_library(gnuradio-channels SHARED ${channels_sources})
target_link_libraries(gnuradio-channels ${channels_libs})
GR_LIBRARY_FOO(gnuradio-channels RUNTIME_COMPONENT "channels_runtime" DEVEL_COMPONENT "channels_devel")
add_dependencies(gnuradio-channels
  channels_generated_includes channels_generated_swigs
  gnuradio-core gnuradio-filter gnuradio-analog gnuradio-blocks)
