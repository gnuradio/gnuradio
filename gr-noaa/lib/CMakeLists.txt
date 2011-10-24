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
    ${GR_NOAA_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_noaa_sources
    noaa_hrpt_decoder.cc
    noaa_hrpt_deframer.cc
    noaa_hrpt_pll_cf.cc
)

list(APPEND noaa_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

add_library(gnuradio-noaa SHARED ${gr_noaa_sources})
target_link_libraries(gnuradio-noaa ${noaa_libs})
GR_LIBRARY_FOO(gnuradio-noaa RUNTIME_COMPONENT "noaa_runtime" DEVEL_COMPONENT "noaa_devel")

########################################################################
# Install header files
########################################################################
install(FILES
    noaa_api.h
    noaa_hrpt_decoder.h
    noaa_hrpt_deframer.h
    noaa_hrpt_pll_cf.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "noaa_devel"
)
