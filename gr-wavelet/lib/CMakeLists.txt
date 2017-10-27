# Copyright 2012,2014 Free Software Foundation, Inc.
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
    ${GR_WAVELET_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${GSL_INCLUDE_DIRS}
)

link_directories(${GR_WAVELET_LIBRARY_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${GSL_LIBRARY_DIRS})
add_definitions(${GSL_DEFINITIONS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND gr_wavelet_sources
    squash_ff_impl.cc
    wavelet_ff_impl.cc
    wvps_ff_impl.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-wavelet.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-wavelet.rc
    @ONLY)

    list(APPEND gr_wavelet_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-wavelet.rc
    )
endif(MSVC)

list(APPEND wavelet_libs
    gnuradio-runtime
    gnuradio-blocks
    ${Boost_LIBRARIES}
    ${WAVELET_LIBRARIES}
    ${GSL_LIBRARIES}
)

add_library(gnuradio-wavelet SHARED ${gr_wavelet_sources})
target_link_libraries(gnuradio-wavelet ${wavelet_libs})

# we need -no-as-needed or else -lgslcblas gets stripped out on newer version of gcc
if(CMAKE_COMPILER_IS_GNUCC AND NOT APPLE)
    SET_TARGET_PROPERTIES(gnuradio-wavelet PROPERTIES LINK_FLAGS "-Wl,--no-as-needed")
endif()

GR_LIBRARY_FOO(gnuradio-wavelet RUNTIME_COMPONENT "wavelet_runtime" DEVEL_COMPONENT "wavelet_devel")
add_dependencies(gnuradio-wavelet
  wavelet_generated_includes wavelet_generated_swigs
  gnuradio-runtime)

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-wavelet APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-wavelet_static STATIC ${gr_wavelet_sources})

  add_dependencies(gnuradio-wavelet_static wavelet_generated_includes
    gnuradio-runtime_static)

  if(NOT WIN32)
    set_target_properties(gnuradio-wavelet_static
      PROPERTIES OUTPUT_NAME gnuradio-wavelet)
  endif(NOT WIN32)

  install(TARGETS gnuradio-wavelet_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "wavelet_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)
