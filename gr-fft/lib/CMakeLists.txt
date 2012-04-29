# Copyright 2012 Free Software Foundation, Inc.
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
    ${GR_FFT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

include_directories(${FFT_INCLUDE_DIRS})
link_directories(${FFT_LIBRARY_DIRS})

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${FFTW3F_INCLUDE_DIRS})
link_directories(${FFTW3F_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND fft_sources
  fft.cc
  fft_vcc_fftw.cc
  fft_vfc_fftw.cc
  goertzel_fc_impl.cc
  goertzel.cc
)

list(APPEND fft_libs
    gnuradio-core
    ${Boost_LIBRARIES}
    ${FFT_LIBRARIES}
    ${FFTW3F_LIBRARIES}
)

add_library(gnuradio-fft SHARED ${fft_sources})
target_link_libraries(gnuradio-fft ${fft_libs})
GR_LIBRARY_FOO(gnuradio-fft RUNTIME_COMPONENT "fft_runtime" DEVEL_COMPONENT "fft_devel")
