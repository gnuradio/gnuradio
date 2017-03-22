# Copyright 2011,2013,2016 Free Software Foundation, Inc.
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
    ${GR_VOCODER_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND gr_vocoder_sources
    alaw_decode_bs_impl.cc
    alaw_encode_sb_impl.cc
    cvsd_decode_bs_impl.cc
    cvsd_encode_sb_impl.cc
    g721_decode_bs_impl.cc
    g721_encode_sb_impl.cc
    g723_24_decode_bs_impl.cc
    g723_24_encode_sb_impl.cc
    g723_40_decode_bs_impl.cc
    g723_40_encode_sb_impl.cc
    ulaw_decode_bs_impl.cc
    ulaw_encode_sb_impl.cc
)
if(LIBCODEC2_FOUND)
  list(APPEND gr_vocoder_sources
    codec2.cc
    codec2_decode_ps_impl.cc
    codec2_encode_sp_impl.cc
    )
endif(LIBCODEC2_FOUND)
if(LIBCODEC2_HAS_FREEDV_API)
  list(APPEND gr_vocoder_sources
    freedv_api.cc
    freedv_rx_ss_impl.cc
    freedv_tx_ss_impl.cc
    )
endif(LIBCODEC2_HAS_FREEDV_API)
if(LIBGSM_FOUND)
  list(APPEND gr_vocoder_sources
    gsm_fr_decode_ps_impl.cc
    gsm_fr_encode_sp_impl.cc
    )
endif(LIBGSM_FOUND)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-vocoder.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.rc
    @ONLY)

    list(APPEND gr_uhd_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.rc
    )
endif(MSVC)

########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
GR_INCLUDE_SUBDIRECTORY(g7xx)

list(APPEND vocoder_libs
    gnuradio-runtime
    ${Boost_LIBRARIES}
)

if(LIBCODEC2_LIBRARIES)
  list(APPEND vocoder_libs ${LIBCODEC2_LIBRARIES})
endif(LIBCODEC2_LIBRARIES)

if(LIBGSM_LIBRARIES)
  list(APPEND vocoder_libs ${LIBGSM_LIBRARIES})
endif(LIBGSM_LIBRARIES)

add_library(gnuradio-vocoder SHARED ${gr_vocoder_sources})
target_link_libraries(gnuradio-vocoder ${vocoder_libs})
GR_LIBRARY_FOO(gnuradio-vocoder)

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-vocoder APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-vocoder_static STATIC ${gr_vocoder_sources})

  if(NOT WIN32)
    set_target_properties(gnuradio-vocoder_static
      PROPERTIES OUTPUT_NAME gnuradio-vocoder)
  endif(NOT WIN32)

  install(TARGETS gnuradio-vocoder_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    )
endif(ENABLE_STATIC_LIBS)
