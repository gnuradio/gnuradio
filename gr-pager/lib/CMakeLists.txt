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
    ${GR_PAGER_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_pager_sources
    pager_flex_frame.cc
    pager_slicer_fb.cc
    pager_flex_sync.cc
    pager_flex_deinterleave.cc
    pager_flex_parse.cc
    pageri_bch3221.cc
    pageri_flex_modes.cc
    pageri_util.cc
)

list(APPEND pager_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

add_library(gnuradio-pager SHARED ${gr_pager_sources})
target_link_libraries(gnuradio-pager ${pager_libs})
GR_LIBRARY_FOO(gnuradio-pager RUNTIME_COMPONENT "pager_runtime" DEVEL_COMPONENT "pager_devel")

########################################################################
# Install header files
########################################################################
install(FILES
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
