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
# generate helper scripts to expand templated files
########################################################################
include(GrPython)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py "
#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_CORE_PYTHONPATH}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_impl_dict2(name, sig, 'filter')
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
expand_cc(fir_filter_XXX_impl              ccc ccf fcc fff fsf scc)
expand_cc(freq_xlating_fir_filter_XXX_impl ccc ccf fcc fcf scf scc)
expand_cc(interp_fir_filter_XXX_impl       ccc ccf fcc fff fsf scc)
expand_cc(rational_resampler_base_XXX_impl ccc ccf fcc fff fsf scc)


########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${GR_FILTER_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${VOLK_INCLUDE_DIRS}
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_FFT_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${FFTW3F_INCLUDE_DIRS}
)

link_directories(${FFT_LIBRARY_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${FFTW3F_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND filter_sources
  adaptive_fir.cc
  fir_filter.cc
  fir_filter_with_buffer.cc
  fft_filter.cc
  firdes.cc
  mmse_fir_interpolator_cc.cc
  mmse_fir_interpolator_ff.cc
  pm_remez.cc
  polyphase_filterbank.cc
  ${generated_sources}
  adaptive_fir_ccc_impl.cc
  adaptive_fir_ccf_impl.cc
  dc_blocker_cc_impl.cc
  dc_blocker_ff_impl.cc
  filter_delay_fc_impl.cc
  fft_filter_ccc_impl.cc
  fft_filter_fff_impl.cc
  fractional_interpolator_cc_impl.cc
  fractional_interpolator_ff_impl.cc
  hilbert_fc_impl.cc
  iir_filter_ffd_impl.cc
  pfb_arb_resampler_ccf_impl.cc
  pfb_arb_resampler_fff_impl.cc
  pfb_channelizer_ccf_impl.cc
  pfb_decimator_ccf_impl.cc
  pfb_interpolator_ccf_impl.cc
  pfb_synthesizer_ccf_impl.cc
  single_pole_iir_filter_cc_impl.cc
  single_pole_iir_filter_ff_impl.cc
  channel_model_impl.cc
)

list(APPEND filter_libs
    gnuradio-core
    gnuradio-fft
    volk
    ${Boost_LIBRARIES}
    ${FFTW3F_LIBRARIES}
)

add_library(gnuradio-filter SHARED ${filter_sources})
target_link_libraries(gnuradio-filter ${filter_libs})
GR_LIBRARY_FOO(gnuradio-filter RUNTIME_COMPONENT "filter_runtime" DEVEL_COMPONENT "filter_devel")
add_dependencies(gnuradio-filter gnuradio-fft filter_generated_includes filter_generated_swigs)


########################################################################
# QA C++ Code for gr-filter
########################################################################
if(ENABLE_TESTING)
  include(GrTest)

  include_directories(${CPPUNIT_INCLUDE_DIRS})
  link_directories(${CPPUNIT_LIBRARY_DIRS})

  list(APPEND test_gr_filter_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_gr_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_firdes.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_fir_filter_with_buffer.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_mmse_fir_interpolator_cc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_mmse_fir_interpolator_ff.cc
    )

  add_executable(test-gr-filter ${test_gr_filter_sources})

  target_link_libraries(
    test-gr-filter
    gnuradio-core
    gnuradio-filter 
    ${Boost_LIBRARIES}
    ${CPPUNIT_LIBRARIES}
  )

  GR_ADD_TEST(test_gr_filter test-gr-filter)
endif(ENABLE_TESTING)
