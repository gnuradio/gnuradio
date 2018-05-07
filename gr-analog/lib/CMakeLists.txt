# Copyright 2012-2014 Free Software Foundation, Inc.
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
    ${CMAKE_CURRENT_BINARY_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GR_ANALOG_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GR_FFT_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Invoke macro to generate various sources and headers
########################################################################
include(GrMiscUtils)
GR_EXPAND_X_CC_H(analog noise_source_X_impl     s i f c)
GR_EXPAND_X_CC_H(analog fastnoise_source_X_impl s i f c)
GR_EXPAND_X_CC_H(analog sig_source_X_impl     s i f c)
GR_EXPAND_X_CC_H(analog random_uniform_source_X_impl b s i)

########################################################################
# Setup library
########################################################################
list(APPEND analog_sources
    ${generated_sources}
    cpm.cc
    squelch_base_cc_impl.cc
    squelch_base_ff_impl.cc
    agc_cc_impl.cc
    agc_ff_impl.cc
    agc2_cc_impl.cc
    agc2_ff_impl.cc
    agc3_cc_impl.cc
    cpfsk_bc_impl.cc
    ctcss_squelch_ff_impl.cc
    dpll_bb_impl.cc
    feedforward_agc_cc_impl.cc
    fmdet_cf_impl.cc
    frequency_modulator_fc_impl.cc
    phase_modulator_fc_impl.cc
    pll_carriertracking_cc_impl.cc
    pll_freqdet_cf_impl.cc
    pll_refout_cc_impl.cc
    probe_avg_mag_sqrd_c_impl.cc
    probe_avg_mag_sqrd_cf_impl.cc
    probe_avg_mag_sqrd_f_impl.cc
    pwr_squelch_cc_impl.cc
    pwr_squelch_ff_impl.cc
    quadrature_demod_cf_impl.cc
    rail_ff_impl.cc
    simple_squelch_cc_impl.cc
)

#Add Windows DLL resource file if using MSVC
IF(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-analog.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-analog.rc
    @ONLY)

    list(APPEND analog_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-analog.rc
    )

ENDIF(MSVC)

list(APPEND analog_libs
    gnuradio-runtime
    gnuradio-blocks
    gnuradio-filter
    ${VOLK_LIBRARIES}
    ${Boost_LIBRARIES}
)

add_library(gnuradio-analog SHARED ${analog_sources})
target_link_libraries(gnuradio-analog ${analog_libs})
GR_LIBRARY_FOO(gnuradio-analog RUNTIME_COMPONENT "analog_runtime" DEVEL_COMPONENT "analog_devel")
add_dependencies(gnuradio-analog analog_generated_includes analog_generated_swigs gnuradio-filter)

if(ENABLE_STATIC_LIBS)
  add_library(gnuradio-analog_static STATIC ${analog_sources})

  add_dependencies(gnuradio-analog_static
    analog_generated_includes
    gnuradio-filter_static)

  if(NOT WIN32)
    set_target_properties(gnuradio-analog_static
      PROPERTIES OUTPUT_NAME gnuradio-analog)
  endif(NOT WIN32)

  install(TARGETS gnuradio-analog_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "analog_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)

########################################################################
# QA C++ Code for gr-analog
########################################################################
if(ENABLE_TESTING)
  include(GrTest)

  include_directories(
    ${GR_ANALOG_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
  )

  list(APPEND test_gr_analog_sources
  )
  list(APPEND GR_TEST_TARGET_DEPS gnuradio-analog)

  foreach(qa_file ${test_gr_analog_sources})
    GR_ADD_CPP_TEST("analog_${qa_file}"
      ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file}
    )
  endforeach(qa_file)
endif(ENABLE_TESTING)
