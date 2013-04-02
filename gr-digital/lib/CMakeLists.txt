# Copyright 2011-2013 Free Software Foundation, Inc.
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
    ${GR_DIGITAL_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GR_ANALOG_INCLUDE_DIRS}
    ${GR_FFT_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${LOG4CPP_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${LOG4CPP_LIBRARY_DIRS})

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
        d = build_utils.standard_impl_dict2(name, sig, 'digital')
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
expand_cc(chunks_to_symbols_XX_impl     bf bc sf sc if ic)

########################################################################
# Setup library
########################################################################
list(APPEND digital_sources
    ${generated_sources}
    additive_scrambler_bb_impl.cc
    binary_slicer_fb_impl.cc
    clock_recovery_mm_cc_impl.cc
    clock_recovery_mm_ff_impl.cc
    cma_equalizer_cc_impl.cc
    constellation.cc
    constellation_decoder_cb_impl.cc
    constellation_receiver_cb_impl.cc
    correlate_access_code_bb_impl.cc
    correlate_access_code_tag_bb_impl.cc
    costas_loop_cc_impl.cc
    cpmmod_bc_impl.cc
    crc32.cc
    crc32_bb_impl.cc
    descrambler_bb_impl.cc
    diff_decoder_bb_impl.cc
    diff_encoder_bb_impl.cc
    diff_phasor_cc_impl.cc
    fll_band_edge_cc_impl.cc
    framer_sink_1_impl.cc
    glfsr.cc
    glfsr_source_b_impl.cc
    glfsr_source_f_impl.cc
    header_payload_demux_impl.cc
    kurtotic_equalizer_cc_impl.cc
    lms_dd_equalizer_cc_impl.cc
    map_bb_impl.cc
    mpsk_receiver_cc_impl.cc
    mpsk_snr_est.cc
    mpsk_snr_est_cc_impl.cc
    ofdm_carrier_allocator_cvc_impl.cc
    ofdm_chanest_vcvc_impl.cc
    ofdm_cyclic_prefixer_impl.cc
    ofdm_equalizer_base.cc
    ofdm_equalizer_simpledfe.cc
    ofdm_equalizer_static.cc
    ofdm_frame_acquisition_impl.cc
    ofdm_frame_equalizer_vcvc_impl.cc
    ofdm_frame_sink_impl.cc
    ofdm_insert_preamble_impl.cc
    ofdm_mapper_bcv_impl.cc
    ofdm_sampler_impl.cc
    ofdm_serializer_vcc_impl.cc
    ofdm_sync_sc_cfb_impl.cc
    packet_header_default.cc
    packet_header_ofdm.cc
    packet_headergenerator_bb_impl.cc
    packet_headerparser_b_impl.cc
    packet_sink_impl.cc
    pfb_clock_sync_ccf_impl.cc
    pfb_clock_sync_fff_impl.cc
    pn_correlator_cc_impl.cc
    probe_density_b_impl.cc
    probe_mpsk_snr_est_c_impl.cc
    scrambler_bb_impl.cc
    simple_correlator_impl.cc
    simple_framer_impl.cc
    header_payload_demux_impl.cc
)

#Add Windows DLL resource file if using MSVC
IF(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-digital.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-digital.rc
    @ONLY)

    list(APPEND gr_digital_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-digital.rc
    )
ENDIF(MSVC)

list(APPEND digital_libs
    volk
    gnuradio-runtime
    gnuradio-filter
    gnuradio-blocks
    gnuradio-analog
    ${Boost_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)


add_library(gnuradio-digital SHARED ${digital_sources})
target_link_libraries(gnuradio-digital ${digital_libs})
GR_LIBRARY_FOO(gnuradio-digital RUNTIME_COMPONENT "digital_runtime" DEVEL_COMPONENT "digital_devel")

add_dependencies(
    gnuradio-digital
    digital_generated_includes
    digital_generated_swigs
    gnuradio-runtime
    gnuradio-filter
    gnuradio-analog
    gnuradio-blocks
)
