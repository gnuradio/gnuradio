# Copyright 2012-2013 Free Software Foundation, Inc.
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

# These are convenience libraries of 3rd party code.
# Change to test for distro provided packages
GR_INCLUDE_SUBDIRECTORY(reed-solomon)
GR_INCLUDE_SUBDIRECTORY(viterbi)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GR_FEC_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
  include_directories(${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gnuradio_fec_sources
    decode_ccsds_27_fb_impl.cc
    encode_ccsds_27_bb_impl.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fec.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.rc
    @ONLY)

    list(APPEND gnuradio_fec_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.rc
    )
endif(MSVC)

list(APPEND gnuradio_fec_libs
    gnuradio-runtime
    ${Boost_LIBRARIES}
)

add_library(gnuradio-fec SHARED ${gnuradio_fec_sources})
target_link_libraries(gnuradio-fec ${gnuradio_fec_libs})
GR_LIBRARY_FOO(gnuradio-fec RUNTIME_COMPONENT "fec_runtime" DEVEL_COMPONENT "fec_devel")

