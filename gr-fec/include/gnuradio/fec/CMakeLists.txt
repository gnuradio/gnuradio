# Copyright 2012,2014 Free Software Foundation, Inc.
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
# Install header files
########################################################################
install(FILES
    api.h
    generic_decoder.h
    generic_encoder.h
    decoder.h
    encoder.h
    tagged_decoder.h
    tagged_encoder.h
    async_decoder.h
    async_encoder.h
    cc_common.h
    cc_decoder.h
    cc_encoder.h
    ccsds_encoder.h
    dummy_encoder.h
    dummy_decoder.h
    repetition_encoder.h
    repetition_decoder.h
    decode_ccsds_27_fb.h
    encode_ccsds_27_bb.h
    rs.h
    viterbi.h
    ber_bf.h
    conv_bit_corr_bb.h
    puncture_bb.h
    puncture_ff.h
    depuncture_bb.h
    cldpc.h
    alist.h
    gf2mat.h
    gf2vec.h
    awgn_bp.h
    ldpc_encoder.h
    ldpc_decoder.h
    polar_encoder.h
    polar_decoder_sc.h
    polar_common.h
    polar_decoder_sc_list.h
    polar_decoder_common.h
    polar_encoder_systematic.h
    polar_decoder_sc_systematic.h
    maxstar.h
    tpc_common.h
    tpc_decoder.h
    tpc_encoder.h DESTINATION ${GR_INCLUDE_DIR}/gnuradio/fec
    COMPONENT "fec_devel"
)

if(GSL_FOUND)
  install(FILES
    fec_mtrx.h
    ldpc_H_matrix.h
    ldpc_G_matrix.h
    ldpc_bit_flip_decoder.h
    ldpc_par_mtrx_encoder.h
    ldpc_gen_mtrx_encoder.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/fec
    COMPONENT "fec_devel"
  )
endif(GSL_FOUND)
