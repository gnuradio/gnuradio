/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef INCLUDED_GR_STREAMS_ENCODE_TURBO_H
#define INCLUDED_GR_STREAMS_ENCODE_TURBO_H

#include <gr_block.h>
#include <gr_streams_encode_convolutional.h>

#if 0
#include <libecc/encoder_turbo_ic1_ic1.h>
#else
#include <libecc/encoder_turbo.h>
#endif

/*!
 * \brief Encode the incoming stream(s) using a turbo encoder
 *
 * input: streams of char, one stream per input as defined by the
 *     instantiated code, using only the right-most justified bit as
 *     the single input bit per input item.
 *
 * output: streams of char, one stream per output as defined by the
 *     instantiated code, using only the right-most justified bit as
 *     the single output bit per output item.
 *
 * n_code_inputs:
 * n_code_outputs: the total number of code inputs and outputs for the
 *     overall turbo encoder (not just the constituent codes).
 *
 * encoders: the constituent encoders to be used; all -should- be
 *     configured with the same "block_size" and "termination", though
 *     from this encoder's perspective it doesn't really matter.
 *
 * interleavers: the interleavers to use before each encoder,
 *     respectively, except the first encoder which will not use an
 *     interleaver.
 */

class gr_streams_encode_turbo;

typedef boost::shared_ptr<gr_streams_encode_turbo>
gr_streams_encode_turbo_sptr;

gr_streams_encode_turbo_sptr
gr_make_streams_encode_turbo
(int n_code_inputs,
 int n_code_outputs,
 const std::vector<gr_streams_encode_convolutional_sptr> &encoders,
 const std::vector<size_t> &interleavers);

class gr_streams_encode_turbo : public gr_block
{
  friend gr_streams_encode_turbo_sptr
  gr_make_streams_encode_turbo
    (int n_code_inputs,
     int n_code_outputs,
     const std::vector<gr_streams_encode_convolutional_sptr> &d_encoders,
     const std::vector<size_t> &d_interleavers);

  gr_streams_encode_turbo
    (int n_code_inputs,
     int n_code_outputs,
     const std::vector<gr_streams_encode_convolutional_sptr> &d_encoders,
     const std::vector<size_t> &d_interleavers);

  std::vector<gr_streams_encode_turbo_sptr> d_encoders;
  std::vector<size_t> d_interleavers;
  size_t d_n_encoders, d_block_size_bits;
  bool d_do_termination;
  encoder_turbo* d_encoder;

public:
  ~gr_streams_encode_turbo ();

  virtual void forecast (int noutput_items,
			 gr_vector_int &ninput_items_required);

  virtual int general_work (int noutput_items,
			    gr_vector_int &ninput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_STREAMS_ENCODE_TURBO_H */
