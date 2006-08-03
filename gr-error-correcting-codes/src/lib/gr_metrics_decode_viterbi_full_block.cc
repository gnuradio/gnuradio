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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_metrics_decode_viterbi_full_block.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <iostream>

gr_metrics_decode_viterbi_full_block_sptr
gr_make_metrics_decode_viterbi_full_block
(int sample_precision,
 int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 bool do_termination,
 int start_memory_state,
 int end_memory_state)
{
  return gr_metrics_decode_viterbi_full_block_sptr
    (new gr_metrics_decode_viterbi_full_block
     (sample_precision,
      frame_size_bits,
      n_code_inputs,
      n_code_outputs,
      code_generator,
      do_termination,
      start_memory_state,
      end_memory_state));
}

gr_metrics_decode_viterbi_full_block_feedback_sptr
gr_make_metrics_decode_viterbi_full_block_feedback
(int sample_precision,
 int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 const std::vector<int> &code_feedback,
 bool do_termination,
 int start_memory_state,
 int end_memory_state)
{
  return gr_metrics_decode_viterbi_full_block_feedback_sptr
    (new gr_metrics_decode_viterbi_full_block
     (sample_precision,
      frame_size_bits,
      n_code_inputs,
      n_code_outputs,
      code_generator,
      code_feedback,
      do_termination,
      start_memory_state,
      end_memory_state));
}

gr_metrics_decode_viterbi_full_block::gr_metrics_decode_viterbi_full_block
(int sample_precision,
 int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 bool do_termination,
 int start_memory_state,
 int end_memory_state)
  : gr_block ("metrics_decode_viterbi_full_block",
	      gr_make_io_signature (0, 0, 0),
	      gr_make_io_signature (0, 0, 0))
{
  d_encoder = new encoder_convolutional_ic1_ic1 (frame_size_bits,
						 n_code_inputs,
						 n_code_outputs,
						 code_generator,
						 do_termination,
						 start_memory_state,
						 end_memory_state);

  setup_io_signatures (sample_precision, n_code_inputs, n_code_outputs);
}

gr_metrics_decode_viterbi_full_block::gr_metrics_decode_viterbi_full_block
(int sample_precision,
 int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 const std::vector<int> &code_feedback,
 bool do_termination,
 int start_memory_state,
 int end_memory_state)
  : gr_block ("metrics_decode_viterbi_full_block_feedback",
	      gr_make_io_signature (0, 0, 0),
	      gr_make_io_signature (0, 0, 0))
{
  d_encoder = new encoder_convolutional_ic1_ic1 (frame_size_bits,
						 n_code_inputs,
						 n_code_outputs,
						 code_generator,
						 code_feedback,
						 do_termination,
						 start_memory_state,
						 end_memory_state);

  setup_io_signatures (sample_precision, n_code_inputs, n_code_outputs);
}

gr_metrics_decode_viterbi_full_block::~gr_metrics_decode_viterbi_full_block
()
{
  delete d_decoder;
  delete d_encoder;
}

void
gr_metrics_decode_viterbi_full_block::setup_io_signatures
(int sample_precision,
 int n_code_inputs,
 int n_code_outputs)
{
  // create the decoder using:
  //
  // the "i1" input model: individual input streams; two per metric
  // type (0-bit, 1-bit), single metric per input item (char, short, long)
  //
  // the "ic1" output model:
  // individual output streams per decoded code input stream;
  // each item is a 'char' type with 1 bit aligned on the LSB.

  d_decoder = new decoder_viterbi_full_block_i1_ic1 (sample_precision,
						     d_encoder);

  // error checking is done in the encoder and decoder classes
  // so just use the parameters as given; will be correct!

  d_n_code_inputs = n_code_inputs;
  d_n_code_outputs = n_code_outputs;

  // output signature is always the same:
  // sizeof (char) with 1 bit per char as the LSB

  set_output_signature (gr_make_io_signature (d_n_code_inputs,
					      d_n_code_inputs,
					      sizeof (char)));

  // determine the input signature element size
  size_t l_input_item_size_bytes;

  if (sample_precision == 0) {
    // float
    l_input_item_size_bytes = sizeof (float);
  } else if (sample_precision <= 8) {
    // use char
    l_input_item_size_bytes = sizeof (char);
  } else if (sample_precision <= 16) {
    // use short
    l_input_item_size_bytes = sizeof (short);
  } else {
    // use long
    l_input_item_size_bytes = sizeof (long);
  }

  set_input_signature (gr_make_io_signature (2*d_n_code_outputs,
					     2*d_n_code_outputs,
					     l_input_item_size_bytes));
}

void gr_metrics_decode_viterbi_full_block::forecast
(int noutput_items,
 gr_vector_int &ninput_items_required)
{
  int ninput_items = d_decoder->compute_n_input_metrics (noutput_items);
  size_t ninputs = ninput_items_required.size();
  for (size_t n = 0; n < ninputs; n++)
    ninput_items_required[n] = ninput_items;
}

int
gr_metrics_decode_viterbi_full_block::general_work
(int noutput_items,
 gr_vector_int &ninput_items,
 gr_vector_const_void_star &input_items,
 gr_vector_void_star &output_items)
{
  // FIXME: compute the actual number of output items (1 bit char's) created.

  size_t t_n_input_items = d_decoder->compute_n_input_metrics (noutput_items);
  size_t t_n_output_items = d_decoder->compute_n_output_bits (t_n_input_items);

  assert (t_n_output_items == ((size_t)noutput_items));

  // "work" is handled by the decoder; which returns the actual number
  // of input items (metrics) used.

  t_n_input_items = d_decoder->decode ((const char**)(&input_items[0]),
				       (char**)(&output_items[0]),
				       noutput_items);

  // consume the number of used input items on all input streams

  consume_each (t_n_input_items);

  // returns number of items written to each output stream

  return (noutput_items);
}
