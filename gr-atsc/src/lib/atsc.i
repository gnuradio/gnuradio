/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

%include "gnuradio.i"				// the common stuff

//load generated python docstrings
%include "atsc_swig_doc.i"

%{
#include <atsc_randomizer.h>
#include <atsc_derandomizer.h>
#include <atsc_rs_encoder.h>
#include <atsc_rs_decoder.h>
#include <atsc_interleaver.h>
#include <atsc_deinterleaver.h>
#include <atsc_trellis_encoder.h>
#include <atsc_viterbi_decoder.h>
#include <atsc_ds_to_softds.h>
#include <atsc_field_sync_mux.h>
#include <atsc_field_sync_demux.h>
#include <atsc_equalizer.h>
#include <atsc_fs_checker.h>
#include <atsc_bit_timing_loop.h>
#include <atsc_fpll.h>
#include <atsc_depad.h>
#include <atsc_pad.h>
%}

%include "atsc_consts.h"

%constant int sizeof_atsc_mpeg_packet = sizeof(atsc_mpeg_packet);
%constant int sizeof_atsc_mpeg_packet_no_sync = sizeof(atsc_mpeg_packet_no_sync);
%constant int sizeof_atsc_mpeg_packet_rs_encoded = sizeof(atsc_mpeg_packet_rs_encoded);
%constant int sizeof_atsc_data_segment = sizeof(atsc_data_segment);
%constant int sizeof_atsc_soft_data_segment = sizeof(atsc_soft_data_segment);

%constant int sizeof_atsc_mpeg_packet_pad = atsc_mpeg_packet::NPAD;
%constant int sizeof_atsc_mpeg_packet_no_sync_pad = atsc_mpeg_packet_no_sync::NPAD;
%constant int sizeof_atsc_mpeg_packet_rs_encoded_pad = atsc_mpeg_packet_rs_encoded::NPAD;
%constant int sizeof_atsc_data_segment_pad = atsc_data_segment::NPAD;
%constant int sizeof_atsc_soft_data_segment_pad = atsc_soft_data_segment::NPAD;

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,randomizer);

atsc_randomizer_sptr atsc_make_randomizer();

class atsc_randomizer : public gr_sync_block
{
  atsc_randomizer();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,derandomizer);

atsc_derandomizer_sptr atsc_make_derandomizer();

class atsc_derandomizer : public gr_sync_block
{
  atsc_derandomizer();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,rs_encoder);

atsc_rs_encoder_sptr atsc_make_rs_encoder();

class atsc_rs_encoder : public gr_sync_block
{
  atsc_rs_encoder();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,rs_decoder);

atsc_rs_decoder_sptr atsc_make_rs_decoder();

class atsc_rs_decoder : public gr_sync_block
{
  atsc_rs_decoder();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,interleaver);

atsc_interleaver_sptr atsc_make_interleaver();

class atsc_interleaver : public gr_sync_block
{
  atsc_interleaver();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,deinterleaver);

atsc_deinterleaver_sptr atsc_make_deinterleaver();

class atsc_deinterleaver : public gr_sync_block
{
  atsc_deinterleaver();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,trellis_encoder);

atsc_trellis_encoder_sptr atsc_make_trellis_encoder();

class atsc_trellis_encoder : public gr_sync_block
{
  atsc_trellis_encoder();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,viterbi_decoder);

atsc_viterbi_decoder_sptr atsc_make_viterbi_decoder();

class atsc_viterbi_decoder : public gr_sync_block
{
  atsc_viterbi_decoder();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,ds_to_softds);

atsc_ds_to_softds_sptr atsc_make_ds_to_softds();

class atsc_ds_to_softds : public gr_sync_block
{
  atsc_ds_to_softds();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,field_sync_mux);

atsc_field_sync_mux_sptr atsc_make_field_sync_mux();

class atsc_field_sync_mux : public gr_sync_block
{
  atsc_field_sync_mux();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,field_sync_demux);

atsc_field_sync_demux_sptr atsc_make_field_sync_demux();

class atsc_field_sync_demux : public gr_block
{
  atsc_field_sync_demux();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,equalizer);

atsc_equalizer_sptr atsc_make_equalizer();

class atsc_equalizer : public gr_sync_block
{
  atsc_equalizer();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,fs_checker);

atsc_fs_checker_sptr atsc_make_fs_checker();

class atsc_fs_checker : public gr_sync_block
{
  atsc_fs_checker();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,bit_timing_loop);

atsc_bit_timing_loop_sptr atsc_make_bit_timing_loop();

class atsc_bit_timing_loop : public gr_block
{
  atsc_bit_timing_loop();

public:
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,fpll);

atsc_fpll_sptr atsc_make_fpll();

class atsc_fpll : public gr_sync_block
{
  atsc_fpll();

public:
  void reset();

};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,depad);

atsc_depad_sptr atsc_make_depad();

class atsc_depad : public gr_sync_interpolator
{
  atsc_depad();

public:
  void reset();

};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(atsc,pad);

atsc_pad_sptr atsc_make_pad();

class atsc_pad : public gr_sync_decimator
{
  atsc_pad();

public:
  void reset();

};

// ----------------------------------------------------------------

