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

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GR_CHANNELS_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GR_ANALOG_INCLUDE_DIRS}
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
list(APPEND channels_sources
  channel_model_impl.cc
  channel_model2_impl.cc
  dynamic_channel_model_impl.cc
  fading_model_impl.cc
  selective_fading_model_impl.cc
  flat_fader_impl.cc
  cfo_model_impl.cc
  sro_model_impl.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-channels.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-channels.rc
    @ONLY)

    list(APPEND channels_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-channels.rc
    )
endif(MSVC)

list(APPEND channels_libs
    gnuradio-runtime
    gnuradio-filter
    gnuradio-analog
    gnuradio-blocks
    ${VOLK_LIBRARIES}
    ${Boost_LIBRARIES}
)

add_library(gnuradio-channels SHARED ${channels_sources})
target_link_libraries(gnuradio-channels ${channels_libs})
GR_LIBRARY_FOO(gnuradio-channels RUNTIME_COMPONENT "channels_runtime" DEVEL_COMPONENT "channels_devel")
add_dependencies(gnuradio-channels
  channels_generated_includes channels_generated_swigs
  gnuradio-runtime gnuradio-filter gnuradio-analog gnuradio-blocks)

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-channels APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-channels_static STATIC ${channels_sources})

  add_dependencies(gnuradio-channels_static
    channels_generated_includes
    gnuradio-runtime_static
    gnuradio-filter_static
    gnuradio-analog_static
    gnuradio-blocks_static)

  if(NOT WIN32)
    set_target_properties(gnuradio-channels_static
      PROPERTIES OUTPUT_NAME gnuradio-channels)
  endif(NOT WIN32)

  install(TARGETS gnuradio-channels_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "channels_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)
