# Copyright 2011,2013 Free Software Foundation, Inc.
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
    ${GR_PAGER_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

link_directories(${Boost_LIBRARY_DIRS})

include_directories(${LOG4CPP_INCLUDE_DIRS})
link_directories(${LOG4CPP_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND pager_sources
    flex_deinterleave_impl.cc
    flex_frame_impl.cc
    flex_parse_impl.cc
    flex_sync_impl.cc
    slicer_fb_impl.cc
    bch3221.cc
    flex_modes.cc
    util.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-pager.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pager.rc
    @ONLY)

    list(APPEND gr_pager_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pager.rc
    )
endif(MSVC)

list(APPEND pager_libs
    gnuradio-runtime
    gnuradio-blocks
    ${Boost_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)

add_library(gnuradio-pager SHARED ${pager_sources})
target_link_libraries(gnuradio-pager ${pager_libs})
GR_LIBRARY_FOO(gnuradio-pager RUNTIME_COMPONENT "pager_runtime" DEVEL_COMPONENT "pager_devel")

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-pager APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-pager_static STATIC ${pager_sources})

  if(NOT WIN32)
    set_target_properties(gnuradio-pager_static
      PROPERTIES OUTPUT_NAME gnuradio-pager)
  endif(NOT WIN32)

  install(TARGETS gnuradio-pager_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "pager_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)
