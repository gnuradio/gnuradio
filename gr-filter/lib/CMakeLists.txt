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
# Setup the include and linker paths
########################################################################
include_directories(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${VOLK_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_filter_sources
    gr_filter_decim_fir.cc
)

list(APPEND filter_libs
    gnuradio-core
    ${Boost_LIBRARIES}
    volk
)

add_library(gnuradio-filter SHARED ${gr_filter_sources})
target_link_libraries(gnuradio-filter ${filter_libs})
GR_LIBRARY_FOO(gnuradio-filter RUNTIME_COMPONENT "filter_runtime" DEVEL_COMPONENT "filter_devel")
