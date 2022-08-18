# Copyright 2012,2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(
    FILES api.h
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
          tpc_encoder.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/fec)

if(GSL_FOUND)
    install(FILES fec_mtrx.h ldpc_H_matrix.h ldpc_G_matrix.h ldpc_bit_flip_decoder.h
                  ldpc_par_mtrx_encoder.h ldpc_gen_mtrx_encoder.h
            DESTINATION ${GR_INCLUDE_DIR}/gnuradio/fec)
endif(GSL_FOUND)
