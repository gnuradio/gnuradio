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
  include_directories(${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

########################################################################
# generate helper scripts to expand templated files
########################################################################
include(GrPython)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py "
#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_RUNTIME_PYTHONPATH}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_impl_dict2(name, sig, 'analog')
        build_utils.expand_template(d, inp)
")

macro(expand_cc root)
  #make a list of all the generated files
  unset(expanded_files_cc)
  unset(expanded_files_h)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_cc ${CMAKE_CURRENT_BINARY_DIR}/${name}.cc)
    list(APPEND expanded_files_h  ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)
  endforeach(sig)
  
  #create a command to generate the source files
  add_custom_command(
    OUTPUT ${expanded_files_cc}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.cc.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.cc.t ${ARGN}
  )

  #create a command to generate the header file
  add_custom_command(
    OUTPUT ${expanded_files_h}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.h.t ${ARGN}
  )
  
  #make source files depends on headers to force generation
  set_source_files_properties(${expanded_files_cc}
    PROPERTIES OBJECT_DEPENDS "${expanded_files_h}"
  )
  
  #install rules for the generated cc files
  list(APPEND generated_sources ${expanded_files_cc})  
  list(APPEND generated_headers ${expanded_files_h})  
endmacro(expand_cc)


########################################################################
# Invoke macro to generate various sources
########################################################################
expand_cc(noise_source_X_impl     s i f c)
expand_cc(fastnoise_source_X_impl s i f c)
expand_cc(sig_source_X_impl     s i f c)

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
    volk
    gnuradio-runtime
    gnuradio-blocks
    gnuradio-filter
    ${Boost_LIBRARIES}
)

add_library(gnuradio-analog SHARED ${analog_sources})
target_link_libraries(gnuradio-analog ${analog_libs})
GR_LIBRARY_FOO(gnuradio-analog RUNTIME_COMPONENT "analog_runtime" DEVEL_COMPONENT "analog_devel")
add_dependencies(gnuradio-analog analog_generated_includes analog_generated_swigs gnuradio-filter)


########################################################################
# QA C++ Code for gr-filter
########################################################################
if(ENABLE_TESTING)
  include(GrTest)

  include_directories(${CPPUNIT_INCLUDE_DIRS})
  link_directories(${CPPUNIT_LIBRARY_DIRS})

  list(APPEND test_gr_analog_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_gr_analog.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_analog.cc
    )

  add_executable(test-gr-analog ${test_gr_analog_sources})

  target_link_libraries(
    test-gr-analog
    gnuradio-runtime
    gnuradio-analog 
    ${Boost_LIBRARIES}
    ${CPPUNIT_LIBRARIES}
  )

  list(APPEND GR_TEST_TARGET_DEPS gnuradio-analog gnuradio-filter gnuradio-fft)

  GR_ADD_TEST(test_gr_analog test-gr-analog)
endif(ENABLE_TESTING)
