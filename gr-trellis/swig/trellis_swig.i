/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#define TRELLIS_API
#define DIGITAL_API

%include "gnuradio.i"

//load generated python docstrings
%include "trellis_swig_doc.i"

%include "gnuradio/digital/metric_type.h"
%include "gnuradio/digital/constellation.h"
%include "gnuradio/trellis/siso_type.h"
%include "gnuradio/trellis/fsm.h"
%include "gnuradio/trellis/interleaver.h"

%{
#include "gnuradio/trellis/constellation_metrics_cf.h"
#include "gnuradio/trellis/permutation.h"
#include "gnuradio/trellis/siso_combined_f.h"
#include "gnuradio/trellis/siso_f.h"
#include "gnuradio/trellis/encoder_bb.h"
#include "gnuradio/trellis/encoder_bs.h"
#include "gnuradio/trellis/encoder_bi.h"
#include "gnuradio/trellis/encoder_ss.h"
#include "gnuradio/trellis/encoder_si.h"
#include "gnuradio/trellis/encoder_ii.h"
#include "gnuradio/trellis/sccc_encoder_bb.h"
#include "gnuradio/trellis/sccc_encoder_bs.h"
#include "gnuradio/trellis/sccc_encoder_bi.h"
#include "gnuradio/trellis/sccc_encoder_ss.h"
#include "gnuradio/trellis/sccc_encoder_si.h"
#include "gnuradio/trellis/sccc_encoder_ii.h"
#include "gnuradio/trellis/pccc_encoder_bb.h"
#include "gnuradio/trellis/pccc_encoder_bs.h"
#include "gnuradio/trellis/pccc_encoder_bi.h"
#include "gnuradio/trellis/pccc_encoder_ss.h"
#include "gnuradio/trellis/pccc_encoder_si.h"
#include "gnuradio/trellis/pccc_encoder_ii.h"
#include "gnuradio/trellis/metrics_s.h"
#include "gnuradio/trellis/metrics_i.h"
#include "gnuradio/trellis/metrics_f.h"
#include "gnuradio/trellis/metrics_c.h"
#include "gnuradio/trellis/pccc_decoder_b.h"
#include "gnuradio/trellis/pccc_decoder_s.h"
#include "gnuradio/trellis/pccc_decoder_i.h"
#include "gnuradio/trellis/pccc_decoder_combined_fb.h"
#include "gnuradio/trellis/pccc_decoder_combined_fs.h"
#include "gnuradio/trellis/pccc_decoder_combined_fi.h"
#include "gnuradio/trellis/pccc_decoder_combined_cb.h"
#include "gnuradio/trellis/pccc_decoder_combined_cs.h"
#include "gnuradio/trellis/pccc_decoder_combined_ci.h"
#include "gnuradio/trellis/viterbi_b.h"
#include "gnuradio/trellis/viterbi_s.h"
#include "gnuradio/trellis/viterbi_i.h"
#include "gnuradio/trellis/viterbi_combined_sb.h"
#include "gnuradio/trellis/viterbi_combined_ss.h"
#include "gnuradio/trellis/viterbi_combined_si.h"
#include "gnuradio/trellis/viterbi_combined_ib.h"
#include "gnuradio/trellis/viterbi_combined_is.h"
#include "gnuradio/trellis/viterbi_combined_ii.h"
#include "gnuradio/trellis/viterbi_combined_fb.h"
#include "gnuradio/trellis/viterbi_combined_fs.h"
#include "gnuradio/trellis/viterbi_combined_fi.h"
#include "gnuradio/trellis/viterbi_combined_cb.h"
#include "gnuradio/trellis/viterbi_combined_cs.h"
#include "gnuradio/trellis/viterbi_combined_ci.h"
#include "gnuradio/trellis/sccc_decoder_b.h"
#include "gnuradio/trellis/sccc_decoder_s.h"
#include "gnuradio/trellis/sccc_decoder_i.h"
#include "gnuradio/trellis/sccc_decoder_combined_fb.h"
#include "gnuradio/trellis/sccc_decoder_combined_fs.h"
#include "gnuradio/trellis/sccc_decoder_combined_fi.h"
#include "gnuradio/trellis/sccc_decoder_combined_cb.h"
#include "gnuradio/trellis/sccc_decoder_combined_cs.h"
#include "gnuradio/trellis/sccc_decoder_combined_ci.h"
%}

%include "gnuradio/trellis/constellation_metrics_cf.h"
%include "gnuradio/trellis/permutation.h"
%include "gnuradio/trellis/siso_combined_f.h"
%include "gnuradio/trellis/siso_f.h"
%include "gnuradio/trellis/encoder_bb.h"
%include "gnuradio/trellis/encoder_bs.h"
%include "gnuradio/trellis/encoder_bi.h"
%include "gnuradio/trellis/encoder_ss.h"
%include "gnuradio/trellis/encoder_si.h"
%include "gnuradio/trellis/encoder_ii.h"
%include "gnuradio/trellis/sccc_encoder_bb.h"
%include "gnuradio/trellis/sccc_encoder_bs.h"
%include "gnuradio/trellis/sccc_encoder_bi.h"
%include "gnuradio/trellis/sccc_encoder_ss.h"
%include "gnuradio/trellis/sccc_encoder_si.h"
%include "gnuradio/trellis/sccc_encoder_ii.h"
%include "gnuradio/trellis/pccc_encoder_bb.h"
%include "gnuradio/trellis/pccc_encoder_bs.h"
%include "gnuradio/trellis/pccc_encoder_bi.h"
%include "gnuradio/trellis/pccc_encoder_ss.h"
%include "gnuradio/trellis/pccc_encoder_si.h"
%include "gnuradio/trellis/pccc_encoder_ii.h"
%include "gnuradio/trellis/metrics_s.h"
%include "gnuradio/trellis/metrics_i.h"
%include "gnuradio/trellis/metrics_f.h"
%include "gnuradio/trellis/metrics_c.h"
%include "gnuradio/trellis/pccc_decoder_b.h"
%include "gnuradio/trellis/pccc_decoder_s.h"
%include "gnuradio/trellis/pccc_decoder_i.h"
%include "gnuradio/trellis/pccc_decoder_combined_fb.h"
%include "gnuradio/trellis/pccc_decoder_combined_fs.h"
%include "gnuradio/trellis/pccc_decoder_combined_fi.h"
%include "gnuradio/trellis/pccc_decoder_combined_cb.h"
%include "gnuradio/trellis/pccc_decoder_combined_cs.h"
%include "gnuradio/trellis/pccc_decoder_combined_ci.h"
%include "gnuradio/trellis/viterbi_b.h"
%include "gnuradio/trellis/viterbi_s.h"
%include "gnuradio/trellis/viterbi_i.h"
%include "gnuradio/trellis/viterbi_combined_sb.h"
%include "gnuradio/trellis/viterbi_combined_ss.h"
%include "gnuradio/trellis/viterbi_combined_si.h"
%include "gnuradio/trellis/viterbi_combined_ib.h"
%include "gnuradio/trellis/viterbi_combined_is.h"
%include "gnuradio/trellis/viterbi_combined_ii.h"
%include "gnuradio/trellis/viterbi_combined_fb.h"
%include "gnuradio/trellis/viterbi_combined_fs.h"
%include "gnuradio/trellis/viterbi_combined_fi.h"
%include "gnuradio/trellis/viterbi_combined_cb.h"
%include "gnuradio/trellis/viterbi_combined_cs.h"
%include "gnuradio/trellis/viterbi_combined_ci.h"
%include "gnuradio/trellis/sccc_decoder_b.h"
%include "gnuradio/trellis/sccc_decoder_s.h"
%include "gnuradio/trellis/sccc_decoder_i.h"
%include "gnuradio/trellis/sccc_decoder_combined_fb.h"
%include "gnuradio/trellis/sccc_decoder_combined_fs.h"
%include "gnuradio/trellis/sccc_decoder_combined_fi.h"
%include "gnuradio/trellis/sccc_decoder_combined_cb.h"
%include "gnuradio/trellis/sccc_decoder_combined_cs.h"
%include "gnuradio/trellis/sccc_decoder_combined_ci.h"

GR_SWIG_BLOCK_MAGIC2(trellis, constellation_metrics_cf);
GR_SWIG_BLOCK_MAGIC2(trellis, permutation);
GR_SWIG_BLOCK_MAGIC2(trellis, siso_combined_f);
GR_SWIG_BLOCK_MAGIC2(trellis, siso_f);
GR_SWIG_BLOCK_MAGIC2(trellis, encoder_bb);
GR_SWIG_BLOCK_MAGIC2(trellis, encoder_bs);
GR_SWIG_BLOCK_MAGIC2(trellis, encoder_bi);
GR_SWIG_BLOCK_MAGIC2(trellis, encoder_ss);
GR_SWIG_BLOCK_MAGIC2(trellis, encoder_si);
GR_SWIG_BLOCK_MAGIC2(trellis, encoder_ii);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_encoder_bb);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_encoder_bs);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_encoder_bi);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_encoder_ss);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_encoder_si);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_encoder_ii);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_encoder_bb);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_encoder_bs);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_encoder_bi);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_encoder_ss);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_encoder_si);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_encoder_ii);
GR_SWIG_BLOCK_MAGIC2(trellis, metrics_s);
GR_SWIG_BLOCK_MAGIC2(trellis, metrics_i);
GR_SWIG_BLOCK_MAGIC2(trellis, metrics_f);
GR_SWIG_BLOCK_MAGIC2(trellis, metrics_c);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_decoder_b);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_decoder_s);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_decoder_i);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_decoder_combined_fb);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_decoder_combined_fs);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_decoder_combined_fi);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_decoder_combined_cb);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_decoder_combined_cs);
GR_SWIG_BLOCK_MAGIC2(trellis, pccc_decoder_combined_ci);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_b);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_s);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_i);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_sb);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_ss);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_si);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_ib);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_is);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_ii);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_fb);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_fs);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_fi);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_cb);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_cs);
GR_SWIG_BLOCK_MAGIC2(trellis, viterbi_combined_ci);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_decoder_b);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_decoder_s);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_decoder_i);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_decoder_combined_fb);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_decoder_combined_fs);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_decoder_combined_fi);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_decoder_combined_cb);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_decoder_combined_cs);
GR_SWIG_BLOCK_MAGIC2(trellis, sccc_decoder_combined_ci);
