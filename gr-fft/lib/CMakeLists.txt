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
    ${GR_FFT_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${FFTW3F_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${FFTW3F_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND fft_sources
  fft.cc
  fft_vcc_fftw.cc
  fft_vfc_fftw.cc
  goertzel_fc_impl.cc
  goertzel.cc
  window.cc
)

if(ENABLE_GR_CTRLPORT)
list(APPEND fft_sources
  ctrlport_probe_psd_impl.cc
)
endif(ENABLE_GR_CTRLPORT)

list(APPEND fft_libs
    gnuradio-runtime
    ${VOLK_LIBRARIES}
    ${Boost_LIBRARIES}
    ${FFTW3F_LIBRARIES}
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fft.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fft.rc
    @ONLY)

    list(APPEND fft_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fft.rc
    )
endif(MSVC)

if(FFTW3F_THREADS_LIBRARIES)
    list(APPEND fft_libs ${FFTW3F_THREADS_LIBRARIES})
    add_definitions("-DFFTW3F_THREADS")
endif()

add_library(gnuradio-fft SHARED ${fft_sources})
target_link_libraries(gnuradio-fft ${fft_libs})
GR_LIBRARY_FOO(gnuradio-fft RUNTIME_COMPONENT "fft_runtime" DEVEL_COMPONENT "fft_devel")

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-fft APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-fft_static STATIC ${fft_sources})

  if(NOT WIN32)
    set_target_properties(gnuradio-fft_static
      PROPERTIES OUTPUT_NAME gnuradio-fft)
  endif(NOT WIN32)

  install(TARGETS gnuradio-fft_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "fft_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)
