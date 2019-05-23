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
# Setup library
########################################################################
add_library(gnuradio-fft
  fft.cc
  fft_vcc_fftw.cc
  fft_vfc_fftw.cc
  goertzel_fc_impl.cc
  goertzel.cc
  window.cc
)

target_link_libraries(gnuradio-fft PUBLIC
  gnuradio-runtime
  fftw3f::fftw3f
  ${GR_VOLK_LIB}
)

target_include_directories(gnuradio-fft
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )

if(ENABLE_GR_CTRLPORT)
target_sources(gnuradio-fft PRIVATE
  ctrlport_probe_psd_impl.cc
)
endif(ENABLE_GR_CTRLPORT)


#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fft.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fft.rc
    @ONLY)

    target_sources(gnuradio-fft PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fft.rc
    )
endif(MSVC)

if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-fft FFTW3f)
endif()

if(ENABLE_TESTING)
  include(GrTest)

  list(APPEND test_gr_fft_sources
    qa_fft_shift
  )
  list(APPEND GR_TEST_TARGET_DEPS gnuradio-fft)

  foreach(qa_file ${test_gr_fft_sources})
    GR_ADD_CPP_TEST("fft_${qa_file}"
      ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file}
    )
  endforeach(qa_file)
endif(ENABLE_TESTING)
