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
INCLUDE_DIRECTORIES(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_NOAA_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
LIST(APPEND gr_noaa_sources
    noaa_hrpt_decoder.cc
    noaa_hrpt_deframer.cc
    noaa_hrpt_pll_cf.cc
)

LIST(APPEND noaa_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

ADD_LIBRARY(gnuradio-noaa SHARED ${gr_noaa_sources})
TARGET_LINK_LIBRARIES(gnuradio-noaa ${noaa_libs})
SET_TARGET_PROPERTIES(gnuradio-noaa PROPERTIES DEFINE_SYMBOL "gnuradio_noaa_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-noaa PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-noaa
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "noaa_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "noaa_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "noaa_runtime" # .dll file
)

########################################################################
# Install header files
########################################################################
INSTALL(FILES
    noaa_api.h
    noaa_hrpt_decoder.h
    noaa_hrpt_deframer.h
    noaa_hrpt_pll_cf.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "noaa_devel"
)
