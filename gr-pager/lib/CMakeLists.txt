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
    ${GR_PAGER_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
LIST(APPEND gr_pager_sources
    pager_flex_frame.cc
    pager_slicer_fb.cc
    pager_flex_sync.cc
    pager_flex_deinterleave.cc
    pager_flex_parse.cc
    pageri_bch3221.cc
    pageri_flex_modes.cc
    pageri_util.cc
)

LIST(APPEND pager_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

ADD_LIBRARY(gnuradio-pager SHARED ${gr_pager_sources})
TARGET_LINK_LIBRARIES(gnuradio-pager ${pager_libs})
SET_TARGET_PROPERTIES(gnuradio-pager PROPERTIES DEFINE_SYMBOL "gnuradio_pager_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-pager PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-pager
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "pager_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "pager_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "pager_runtime" # .dll file
)

########################################################################
# Install header files
########################################################################
INSTALL(FILES
    pager_api.h
    pager_slicer_fb.h
    pager_flex_sync.h
    pager_flex_deinterleave.h
    pager_flex_parse.h
    pager_flex_frame.h
    pageri_bch3221.h
    pageri_flex_modes.h
    pageri_util.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "pager_devel"
)
