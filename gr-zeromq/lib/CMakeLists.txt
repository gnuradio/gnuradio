# Copyright 2013 Free Software Foundation, Inc.
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
    ${GR_ZEROMQ_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${ZEROMQ_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND zeromq_sources
  base_impl.cc
  pub_sink_impl.cc
  pub_msg_sink_impl.cc
  sub_source_impl.cc
  sub_msg_source_impl.cc
  pull_source_impl.cc
  pull_msg_source_impl.cc
  push_sink_impl.cc
  push_msg_sink_impl.cc
  rep_sink_impl.cc
  rep_msg_sink_impl.cc
  req_source_impl.cc
  req_msg_source_impl.cc
  tag_headers.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-zeromq.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.rc
    @ONLY)

    list(APPEND gr_zeromq_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.rc
    )
endif(MSVC)

list(APPEND zeromq_libs
    gnuradio-runtime
    ${Boost_LIBRARIES}
    ${ZEROMQ_LIBRARIES}
)

add_library(gnuradio-zeromq SHARED ${zeromq_sources})
target_link_libraries(gnuradio-zeromq ${zeromq_libs})
GR_LIBRARY_FOO(gnuradio-zeromq RUNTIME_COMPONENT "zeromq_runtime" DEVEL_COMPONENT "zeromq_devel")

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-zeromq APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-zeromq_static STATIC ${zeromq_sources})

  if(NOT WIN32)
    set_target_properties(gnuradio-zeromq_static
      PROPERTIES OUTPUT_NAME gnuradio-zeromq)
  endif(NOT WIN32)

  install(TARGETS gnuradio-zeromq_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "zeromq_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)
