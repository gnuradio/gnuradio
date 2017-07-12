/*
 * Copyright 2011-2016 Free Software Foundation, Inc.
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

#define DIGITAL_API
#define ANALOG_API
#define BLOCKS_API

%include "gnuradio.i"
%include "stdint.i"

//load generated python docstrings
%include "digital_swig0_doc.i"

%include "gnuradio/analog/cpm.h"

%{
#include <gnuradio/blocks/control_loop.h>
%}
%include <gnuradio/blocks/control_loop.h>

%{
#include "gnuradio/digital/binary_slicer_fb.h"
#include "gnuradio/digital/cma_equalizer_cc.h"
#include "gnuradio/digital/constellation.h"
#include "gnuradio/digital/constellation_decoder_cb.h"
#include "gnuradio/digital/constellation_receiver_cb.h"
#include "gnuradio/digital/constellation_soft_decoder_cf.h"
#include "gnuradio/digital/corr_est_cc.h"
#include "gnuradio/digital/correlate_access_code_bb.h"
#include "gnuradio/digital/correlate_access_code_tag_bb.h"
#include "gnuradio/digital/correlate_access_code_tag_ff.h"
#include "gnuradio/digital/correlate_access_code_bb_ts.h"
#include "gnuradio/digital/correlate_access_code_ff_ts.h"
#include "gnuradio/digital/correlate_and_sync_cc.h"
#include "gnuradio/digital/costas_loop_cc.h"
#include "gnuradio/digital/cpmmod_bc.h"
#include "gnuradio/digital/crc32.h"
#include "gnuradio/digital/crc32_bb.h"
#include "gnuradio/digital/crc32_async_bb.h"
#include "gnuradio/digital/descrambler_bb.h"
#include "gnuradio/digital/lms_dd_equalizer_cc.h"
#include "gnuradio/digital/ofdm_carrier_allocator_cvc.h"
#include "gnuradio/digital/ofdm_chanest_vcvc.h"
#include "gnuradio/digital/ofdm_cyclic_prefixer.h"
#include "gnuradio/digital/ofdm_equalizer_base.h"
#include "gnuradio/digital/ofdm_equalizer_simpledfe.h"
#include "gnuradio/digital/ofdm_equalizer_static.h"
#include "gnuradio/digital/ofdm_frame_acquisition.h"
#include "gnuradio/digital/ofdm_frame_equalizer_vcvc.h"
#include "gnuradio/digital/ofdm_frame_sink.h"
#include "gnuradio/digital/ofdm_insert_preamble.h"
#include "gnuradio/digital/ofdm_mapper_bcv.h"
#include "gnuradio/digital/ofdm_sampler.h"
#include "gnuradio/digital/ofdm_serializer_vcc.h"
#include "gnuradio/digital/ofdm_sync_sc_cfb.h"
%}

%include "gnuradio/digital/binary_slicer_fb.h"
%include "gnuradio/digital/cma_equalizer_cc.h"
%include "gnuradio/digital/constellation.h"
%include "gnuradio/digital/constellation_decoder_cb.h"
%include "gnuradio/digital/constellation_receiver_cb.h"
%include "gnuradio/digital/constellation_soft_decoder_cf.h"
%include "gnuradio/digital/corr_est_cc.h"
%include "gnuradio/digital/correlate_access_code_bb.h"
%include "gnuradio/digital/correlate_access_code_tag_bb.h"
%include "gnuradio/digital/correlate_access_code_tag_ff.h"
%include "gnuradio/digital/correlate_access_code_bb_ts.h"
%include "gnuradio/digital/correlate_access_code_ff_ts.h"
%include "gnuradio/digital/correlate_and_sync_cc.h"
%include "gnuradio/digital/costas_loop_cc.h"
%include "gnuradio/digital/cpmmod_bc.h"
%include "gnuradio/digital/crc32.h"
%include "gnuradio/digital/crc32_bb.h"
%include "gnuradio/digital/crc32_async_bb.h"
%include "gnuradio/digital/descrambler_bb.h"
%include "gnuradio/digital/lms_dd_equalizer_cc.h"
%include "gnuradio/digital/ofdm_carrier_allocator_cvc.h"
%include "gnuradio/digital/ofdm_chanest_vcvc.h"
%include "gnuradio/digital/ofdm_cyclic_prefixer.h"
%include "gnuradio/digital/ofdm_equalizer_base.h"
%include "gnuradio/digital/ofdm_equalizer_simpledfe.h"
%include "gnuradio/digital/ofdm_equalizer_static.h"
%include "gnuradio/digital/ofdm_frame_acquisition.h"
%include "gnuradio/digital/ofdm_frame_equalizer_vcvc.h"
%include "gnuradio/digital/ofdm_frame_sink.h"
%include "gnuradio/digital/ofdm_insert_preamble.h"
%include "gnuradio/digital/ofdm_mapper_bcv.h"
%include "gnuradio/digital/ofdm_sampler.h"
%include "gnuradio/digital/ofdm_serializer_vcc.h"
%include "gnuradio/digital/ofdm_sync_sc_cfb.h"

GR_SWIG_BLOCK_MAGIC2(digital, binary_slicer_fb);
GR_SWIG_BLOCK_MAGIC2(digital, cma_equalizer_cc);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_decoder_cb);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_receiver_cb);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_soft_decoder_cf);
GR_SWIG_BLOCK_MAGIC2(digital, corr_est_cc);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_bb);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_tag_bb);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_tag_ff);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_bb_ts);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_ff_ts);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_and_sync_cc);
GR_SWIG_BLOCK_MAGIC2(digital, costas_loop_cc);
GR_SWIG_BLOCK_MAGIC2(digital, crc32_bb);
GR_SWIG_BLOCK_MAGIC2(digital, crc32_async_bb);
GR_SWIG_BLOCK_MAGIC2(digital, cpmmod_bc);
GR_SWIG_BLOCK_MAGIC2(digital, descrambler_bb);
GR_SWIG_BLOCK_MAGIC2(digital, lms_dd_equalizer_cc);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_carrier_allocator_cvc);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_chanest_vcvc);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_cyclic_prefixer);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_frame_acquisition);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_frame_equalizer_vcvc);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_frame_sink);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_insert_preamble);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_mapper_bcv);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_sampler);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_serializer_vcc);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_sync_sc_cfb);

GR_SWIG_BLOCK_MAGIC_FACTORY(digital, cpmmod_bc, gmskmod_bc);

// Properly package up non-block objects
%include "constellation.i"
%include "ofdm_equalizer.i"
