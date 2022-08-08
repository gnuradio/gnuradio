# Copyright 2012-2014,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# These are convenience libraries of 3rd party code.
# Change to test for distro provided packages

########################################################################
# Setup library
########################################################################
add_library(gnuradio-fec
  generic_decoder.cc
  generic_encoder.cc
  decoder_impl.cc
  encoder_impl.cc
  tagged_decoder_impl.cc
  tagged_encoder_impl.cc
  async_decoder_impl.cc
  async_encoder_impl.cc
  cc_decoder_impl.cc
  cc_encoder_impl.cc
  ccsds_encoder_impl.cc
  dummy_decoder_impl.cc
  dummy_encoder_impl.cc
  repetition_decoder_impl.cc
  repetition_encoder_impl.cc
  decode_ccsds_27_fb_impl.cc
  encode_ccsds_27_bb_impl.cc
  ber_bf_impl.cc
  conv_bit_corr_bb_impl.cc
  puncture_bb_impl.cc
  puncture_ff_impl.cc
  depuncture_bb_impl.cc
  ldpc_encoder_impl.cc
  ldpc_decoder.cc
  cldpc.cc
  awgn_bp.cc
  gf2vec.cc
  gf2mat.cc
  alist.cc
  tpc_common.cc
  tpc_decoder.cc
  tpc_encoder.cc
  polar_encoder.cc
  polar_decoder_sc.cc
  polar_common.cc
  polar_decoder_sc_list.cc
  polar_decoder_common.cc
  scl_list.cc
  polar_encoder_systematic.cc
  polar_decoder_sc_systematic.cc
  )
target_include_directories(gnuradio-fec
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )

add_subdirectory(reed-solomon)

target_sources(gnuradio-fec PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/viterbi/metrics.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/viterbi/tab.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/viterbi/viterbi.cc
  )

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fec.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.rc
    @ONLY)

    target_sources(gnuradio-fec PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.rc
    )
endif(MSVC)

target_link_libraries(gnuradio-fec PUBLIC
    gnuradio-runtime
    gnuradio-blocks
)

if(ENABLE_COMMON_PCH)
  target_link_libraries(gnuradio-fec PRIVATE common-precompiled-headers)
endif()

# Address linker issues with std::filesystem on Centos 8 and Debian
target_link_libraries(gnuradio-fec PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)

# Only include the LDPC work if we have GSL installed
if(GSL_FOUND)
  target_link_libraries(gnuradio-fec PUBLIC GSL::gsl)
  target_sources(gnuradio-fec PRIVATE
    ldpc_bit_flip_decoder_impl.cc
    ldpc_par_mtrx_encoder_impl.cc
    ldpc_gen_mtrx_encoder_impl.cc
    ldpc_H_matrix_impl.cc
    ldpc_G_matrix_impl.cc
    fec_mtrx_impl.cc
    )
endif(GSL_FOUND)

if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-fec)
endif()
