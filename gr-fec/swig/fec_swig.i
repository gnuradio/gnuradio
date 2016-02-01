/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#define FEC_API

%include "gnuradio.i"

//load generated python docstrings
%include "fec_swig_doc.i"

%include "gnuradio/fec/cc_common.h"

%nodefaultctor gr::fec::generic_encoder;
%template(generic_encoder_sptr) boost::shared_ptr<gr::fec::generic_encoder>;

%nodefaultctor gr::fec::generic_decoder;
%template(generic_decoder_sptr) boost::shared_ptr<gr::fec::generic_decoder>;

%{
#include "gnuradio/fec/generic_decoder.h"
#include "gnuradio/fec/generic_encoder.h"
#include "gnuradio/fec/decoder.h"
#include "gnuradio/fec/encoder.h"
#include "gnuradio/fec/tagged_decoder.h"
#include "gnuradio/fec/tagged_encoder.h"
#include "gnuradio/fec/async_decoder.h"
#include "gnuradio/fec/async_encoder.h"
#include "gnuradio/fec/cc_decoder.h"
#include "gnuradio/fec/cc_encoder.h"
#include "gnuradio/fec/ccsds_encoder.h"
#include "gnuradio/fec/dummy_decoder.h"
#include "gnuradio/fec/dummy_encoder.h"
#include "gnuradio/fec/repetition_decoder.h"
#include "gnuradio/fec/repetition_encoder.h"
#include "gnuradio/fec/decode_ccsds_27_fb.h"
#include "gnuradio/fec/encode_ccsds_27_bb.h"
#include "gnuradio/fec/ber_bf.h"
#include "gnuradio/fec/conv_bit_corr_bb.h"
#include "gnuradio/fec/puncture_bb.h"
#include "gnuradio/fec/puncture_ff.h"
#include "gnuradio/fec/depuncture_bb.h"
#include "gnuradio/fec/ldpc_encoder.h"
#include "gnuradio/fec/ldpc_decoder.h"
#include "gnuradio/fec/tpc_encoder.h"
#include "gnuradio/fec/tpc_decoder.h"
#include "gnuradio/fec/polar_encoder.h"
#include "gnuradio/fec/polar_decoder_sc.h"
#include "gnuradio/fec/polar_common.h"
#include "gnuradio/fec/polar_decoder_sc_list.h"
#include "gnuradio/fec/polar_decoder_common.h"
#include "gnuradio/fec/polar_encoder_systematic.h"
#include "gnuradio/fec/polar_decoder_sc_systematic.h"
%}

%include "gnuradio/fec/generic_decoder.h"
%include "gnuradio/fec/generic_encoder.h"
%include "gnuradio/fec/decoder.h"
%include "gnuradio/fec/encoder.h"
%include "gnuradio/fec/tagged_decoder.h"
%include "gnuradio/fec/tagged_encoder.h"
%include "gnuradio/fec/async_decoder.h"
%include "gnuradio/fec/async_encoder.h"
%include "gnuradio/fec/cc_decoder.h"
%include "gnuradio/fec/cc_encoder.h"
%include "gnuradio/fec/ccsds_encoder.h"
%include "gnuradio/fec/dummy_decoder.h"
%include "gnuradio/fec/dummy_encoder.h"
%include "gnuradio/fec/repetition_decoder.h"
%include "gnuradio/fec/repetition_encoder.h"
%include "gnuradio/fec/decode_ccsds_27_fb.h"
%include "gnuradio/fec/encode_ccsds_27_bb.h"
%include "gnuradio/fec/ber_bf.h"
%include "gnuradio/fec/conv_bit_corr_bb.h"
%include "gnuradio/fec/puncture_bb.h"
%include "gnuradio/fec/puncture_ff.h"
%include "gnuradio/fec/depuncture_bb.h"
%include "gnuradio/fec/tpc_encoder.h"
%include "gnuradio/fec/tpc_decoder.h"
%include "gnuradio/fec/polar_encoder.h"
%include "gnuradio/fec/polar_decoder_sc.h"
%include "gnuradio/fec/polar_common.h"
%include "gnuradio/fec/polar_decoder_sc_list.h"
%include "gnuradio/fec/polar_decoder_common.h"
%include "gnuradio/fec/polar_encoder_systematic.h"
%include "gnuradio/fec/polar_decoder_sc_systematic.h"
%include "gnuradio/fec/ldpc_encoder.h"


#ifdef GSL_FOUND
%{
#include "gnuradio/fec/fec_mtrx.h"
#include "gnuradio/fec/ldpc_H_matrix.h"
#include "gnuradio/fec/ldpc_G_matrix.h"
#include "gnuradio/fec/ldpc_gen_mtrx_encoder.h"
#include "gnuradio/fec/ldpc_par_mtrx_encoder.h"
#include "gnuradio/fec/ldpc_bit_flip_decoder.h"
#include "gnuradio/fec/ldpc_decoder.h"
%}

%include "gnuradio/fec/fec_mtrx.h"
%include "gnuradio/fec/ldpc_H_matrix.h"
%include "gnuradio/fec/ldpc_G_matrix.h"
%include "gnuradio/fec/ldpc_gen_mtrx_encoder.h"
%include "gnuradio/fec/ldpc_par_mtrx_encoder.h"
%include "gnuradio/fec/ldpc_bit_flip_decoder.h"
%include "gnuradio/fec/ldpc_decoder.h"

%include "ldpc.i"
#endif /* GSL_FOUND */

GR_SWIG_BLOCK_MAGIC2(fec, decoder);
GR_SWIG_BLOCK_MAGIC2(fec, encoder);
GR_SWIG_BLOCK_MAGIC2(fec, tagged_decoder);
GR_SWIG_BLOCK_MAGIC2(fec, tagged_encoder);
GR_SWIG_BLOCK_MAGIC2(fec, async_decoder);
GR_SWIG_BLOCK_MAGIC2(fec, async_encoder);
GR_SWIG_BLOCK_MAGIC2(fec, decode_ccsds_27_fb);
GR_SWIG_BLOCK_MAGIC2(fec, encode_ccsds_27_bb);
GR_SWIG_BLOCK_MAGIC2(fec, ber_bf);
GR_SWIG_BLOCK_MAGIC2(fec, conv_bit_corr_bb);
GR_SWIG_BLOCK_MAGIC2(fec, puncture_bb);
GR_SWIG_BLOCK_MAGIC2(fec, puncture_ff);
GR_SWIG_BLOCK_MAGIC2(fec, depuncture_bb);
