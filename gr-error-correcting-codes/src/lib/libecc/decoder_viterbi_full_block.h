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

#ifndef INCLUDED_DECODER_VITERBI_FULL_BLOCK_H
#define INCLUDED_DECODER_VITERBI_FULL_BLOCK_H

#include "decoder_viterbi.h"

class decoder_viterbi_full_block : public decoder_viterbi
{
/*!
 * \brief Decode the incoming streams using a Viterbi-style decoder,
 *     doing full trellis block decoding before putting out any
 *     decoded bits.
 *
 * input: streams of metrics, two per code output: one for the 0-bit
 *     metrics and the other for the 1-bit metric.
 *
 * output: stream(s) of output bits
 */

public:
  decoder_viterbi_full_block (int sample_precision,
			      encoder_convolutional* l_encoder);

  virtual ~decoder_viterbi_full_block ();

  virtual size_t compute_n_input_items (size_t n_output_bits);
  virtual size_t compute_n_output_bits (size_t n_input_items);

protected:
  virtual void decode_private ();
  virtual void update_traceback__up (size_t from_state_ndx,
				     size_t to_state_ndx,
				     size_t l_input);
  virtual void update_traceback__middle ();
  virtual void update_traceback__term ();

/*
 * traceback_t: used to store all encode-input bits for
 *     all possible paths, when computing all trellis bits before
 *     determining the ML decode-output sequence.
 *
 * d_prev: the connection to the previous bit's traceback structure
 *
 * d_inputs: the inputs (one per bit) for this connection
 */

  typedef struct traceback_t {
    struct traceback_t *d_prev;
    int d_inputs;
  } traceback_t, *traceback_t_ptr, **traceback_t_hdl;

/*
 * d_n_total_inputs_per_stream: how many bits to store for each
 *     state to determine the best decoder-output (encoder-input) bits 
 */
  size_t d_n_inputs_per_stream;
  traceback_t_hdl d_out_buf;
};

#endif /* INCLUDED_DECODER_VITERBI_FULL_BLOCK_H */
