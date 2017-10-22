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

# These are convenience libraries of 3rd party code.
# Change to test for distro provided packages
GR_INCLUDE_SUBDIRECTORY(reed-solomon)
GR_INCLUDE_SUBDIRECTORY(viterbi)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GR_FEC_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

link_directories(
  ${Boost_LIBRARY_DIRS}
)

########################################################################
# Setup library
########################################################################
list(APPEND gnuradio_fec_sources
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

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fec.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.rc
    @ONLY)

    list(APPEND gnuradio_fec_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.rc
    )
endif(MSVC)

list(APPEND gnuradio_fec_libs
    gnuradio-blocks
    gnuradio-runtime
    ${VOLK_LIBRARIES}
    ${Boost_LIBRARIES}
)

# Only include the LDPC work if we have GSL installed
if(GSL_FOUND)
  include_directories(${GSL_INCLUDE_DIRS})
  link_directories(${GSL_LIBRARY_DIRS})
  list(APPEND gnuradio_fec_libs ${GSL_LDFLAGS})

  list(APPEND gnuradio_fec_sources
    ldpc_bit_flip_decoder_impl.cc
    ldpc_par_mtrx_encoder_impl.cc
    ldpc_gen_mtrx_encoder_impl.cc
    ldpc_H_matrix_impl.cc
    ldpc_G_matrix_impl.cc
    fec_mtrx_impl.cc
    )
endif(GSL_FOUND)

add_library(gnuradio-fec SHARED ${gnuradio_fec_sources})
target_link_libraries(gnuradio-fec ${gnuradio_fec_libs})
GR_LIBRARY_FOO(gnuradio-fec RUNTIME_COMPONENT "fec_runtime" DEVEL_COMPONENT "fec_devel")

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-fec APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-fec_static STATIC ${gnuradio_fec_sources})

  if(NOT WIN32)
    set_target_properties(gnuradio-fec_static
      PROPERTIES OUTPUT_NAME gnuradio-fec)
  endif(NOT WIN32)

  install(TARGETS gnuradio-fec_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "fec_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)
