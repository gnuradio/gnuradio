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
    ${GR_UHD_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(${UHD_INCLUDE_DIRS})
LINK_DIRECTORIES(${UHD_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
LIST(APPEND gr_uhd_sources
    gr_uhd_usrp_source.cc
    gr_uhd_usrp_sink.cc
)

LIST(APPEND uhd_libs
    gnuradio-core
    ${Boost_LIBRARIES}
    ${UHD_LIBRARIES}
)

ADD_LIBRARY(gnuradio-uhd SHARED ${gr_uhd_sources})
TARGET_LINK_LIBRARIES(gnuradio-uhd ${uhd_libs})
SET_TARGET_PROPERTIES(gnuradio-uhd PROPERTIES DEFINE_SYMBOL "gnuradio_uhd_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-uhd PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-uhd
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "uhd_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "uhd_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "uhd_runtime" # .dll file
)
