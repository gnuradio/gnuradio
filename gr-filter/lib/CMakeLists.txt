# Copyright (C) 2012-2014,2017 Free Software Foundation, Inc.
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
# Invoke macro to generate various sources and headers
########################################################################
include(GrMiscUtils)
GR_EXPAND_X_CC_H(filter fir_filter_XXX_impl              ccc ccf fcc fff fsf scc)
GR_EXPAND_X_CC_H(filter freq_xlating_fir_filter_XXX_impl ccc ccf fcc fcf scf scc)
GR_EXPAND_X_CC_H(filter interp_fir_filter_XXX_impl       ccc ccf fcc fff fsf scc)
GR_EXPAND_X_CC_H(filter rational_resampler_base_XXX_impl ccc ccf fcc fff fsf scc)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GR_FFT_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(
  ${Boost_LIBRARY_DIRS}
)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND filter_sources
  fir_filter.cc
  fir_filter_with_buffer.cc
  fft_filter.cc
  firdes.cc
  iir_filter.cc
  mmse_fir_interpolator_cc.cc
  mmse_fir_interpolator_ff.cc
  mmse_interp_differentiator_cc.cc
  mmse_interp_differentiator_ff.cc
  pm_remez.cc
  polyphase_filterbank.cc
  ${generated_sources}
  dc_blocker_cc_impl.cc
  dc_blocker_ff_impl.cc
  filter_delay_fc_impl.cc
  filterbank.cc
  filterbank_vcvcf_impl.cc
  fft_filter_ccc_impl.cc
  fft_filter_ccf_impl.cc
  fft_filter_fff_impl.cc
  fractional_interpolator_cc_impl.cc
  fractional_interpolator_ff_impl.cc
  fractional_resampler_cc_impl.cc
  fractional_resampler_ff_impl.cc
  hilbert_fc_impl.cc
  iir_filter_ffd_impl.cc
  iir_filter_ccc_impl.cc
  iir_filter_ccf_impl.cc
  iir_filter_ccd_impl.cc
  iir_filter_ccz_impl.cc
  pfb_arb_resampler.cc
  pfb_arb_resampler_ccf_impl.cc
  pfb_arb_resampler_ccc_impl.cc
  pfb_arb_resampler_fff_impl.cc
  pfb_channelizer_ccf_impl.cc
  pfb_decimator_ccf_impl.cc
  pfb_interpolator_ccf_impl.cc
  pfb_synthesizer_ccf_impl.cc
  single_pole_iir_filter_cc_impl.cc
  single_pole_iir_filter_ff_impl.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-filter.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.rc
    @ONLY)

    list(APPEND filter_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.rc
    )
endif(MSVC)

list(APPEND filter_libs
    gnuradio-runtime
    gnuradio-fft
    gnuradio-blocks
    ${VOLK_LIBRARIES}
    ${Boost_LIBRARIES}
)


add_library(gnuradio-filter SHARED ${filter_sources})
target_link_libraries(gnuradio-filter ${filter_libs})
GR_LIBRARY_FOO(gnuradio-filter RUNTIME_COMPONENT "filter_runtime" DEVEL_COMPONENT "filter_devel")
add_dependencies(gnuradio-filter
  filter_generated_includes filter_generated_swigs
  gnuradio-runtime gnuradio-fft)

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-filter APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-filter_static STATIC ${filter_sources})

  add_dependencies(gnuradio-filter_static
    filter_generated_includes
    gnuradio-runtime_static gnuradio-fft_static)

  if(NOT WIN32)
    set_target_properties(gnuradio-filter_static
      PROPERTIES OUTPUT_NAME gnuradio-filter)
  endif(NOT WIN32)

  install(TARGETS gnuradio-filter_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "filter_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)


########################################################################
# QA C++ Code for gr-filter
########################################################################
if(ENABLE_TESTING)
  include(GrTest)

  include_directories(
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
  )

  list(APPEND test_gr_filter_sources
    qa_firdes.cc
    qa_fir_filter_with_buffer.cc
    qa_mmse_fir_interpolator_cc.cc
    qa_mmse_fir_interpolator_ff.cc
    qa_mmse_interp_differentiator_cc.cc
    qa_mmse_interp_differentiator_ff.cc
  )
  list(APPEND GR_TEST_TARGET_DEPS gnuradio-filter gnuradio-fft)

  foreach(qa_file ${test_gr_filter_sources})
    GR_ADD_CPP_TEST("filter_${qa_file}"
      ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file}
    )
  endforeach(qa_file)
endif(ENABLE_TESTING)
