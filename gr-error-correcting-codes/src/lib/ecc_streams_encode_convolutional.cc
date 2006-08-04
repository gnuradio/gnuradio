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

#include <ecc_streams_encode_convolutional.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <iostream>

ecc_streams_encode_convolutional_sptr 
ecc_make_streams_encode_convolutional
(int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 bool do_termination,
 int start_memory_state,
 int end_memory_state)
{
  return ecc_streams_encode_convolutional_sptr
    (new ecc_streams_encode_convolutional (frame_size_bits,
					  n_code_inputs,
					  n_code_outputs,
					  code_generator,
					  do_termination,
					  start_memory_state,
					  end_memory_state));
}

ecc_streams_encode_convolutional_sptr 
ecc_make_streams_encode_convolutional_feedback
(int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 const std::vector<int> &code_feedback,
 bool do_termination,
 int start_memory_state,
 int end_memory_state)
{
  return ecc_streams_encode_convolutional_sptr
    (new ecc_streams_encode_convolutional (frame_size_bits,
					  n_code_inputs,
					  n_code_outputs,
					  code_generator,
					  code_feedback,
					  do_termination,
					  start_memory_state,
					  end_memory_state));
}

ecc_streams_encode_convolutional::ecc_streams_encode_convolutional
(int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generators,
 bool do_termination,
 int start_memory_state,
 int end_memory_state)
  : gr_block ("streams_encode_convolutional",
	      gr_make_io_signature (0, 0, 0),
	      gr_make_io_signature (0, 0, 0))
{
  // error checking is done by the encoder class itself;
  // just pass items on here.

  d_encoder = new encoder_convolutional (frame_size_bits,
					 n_code_inputs,
					 n_code_outputs,
					 code_generators,
					 do_termination,
					 start_memory_state,
					 end_memory_state);

  setup_io_signatures (n_code_inputs, n_code_outputs);
}

ecc_streams_encode_convolutional::ecc_streams_encode_convolutional
(int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generators,
 const std::vector<int> &code_feedback,
 bool do_termination,
 int start_memory_state,
 int end_memory_state)
  : gr_block ("streams_encode_convolutional_feedback",
	      gr_make_io_signature (0, 0, 0),
	      gr_make_io_signature (0, 0, 0))
{
  // error checking is done by the encoder class itself;
  // just pass items on here.

  d_encoder = new encoder_convolutional (frame_size_bits,
					 n_code_inputs,
					 n_code_outputs,
					 code_generators,
					 code_feedback,
					 do_termination,
					 start_memory_state,
					 end_memory_state);

  setup_io_signatures (n_code_inputs, n_code_outputs);
}

void
ecc_streams_encode_convolutional::setup_io_signatures
(int n_code_inputs,
 int n_code_outputs)
{
  // create the correct input signature; 1 bit per input char

  d_in_buf = new code_input_ic1l (n_code_inputs);
  set_input_signature (gr_make_io_signature (n_code_inputs,
					     n_code_inputs,
					     sizeof (char)));

  // create the correct output signature; 1 bit per output char

  d_out_buf = new code_output_ic1l (n_code_outputs);
  set_output_signature (gr_make_io_signature (n_code_outputs,
					      n_code_outputs,
					      sizeof (char)));
}

ecc_streams_encode_convolutional::~ecc_streams_encode_convolutional
()
{
  delete d_encoder;
  d_encoder = 0;
  delete d_in_buf;
  d_in_buf = 0;
  delete d_out_buf;
  d_out_buf = 0;
}

/*
 * Compute the number of input items needed to produce 'n_output'
 * bits.  Handled internally by the encoder.
 */

void ecc_streams_encode_convolutional::forecast
(int noutput_items,
 gr_vector_int &ninput_items_required)
{
  int ninput_items = d_encoder->compute_n_input_bits (noutput_items);
  size_t ninputs = ninput_items_required.size();
  for (size_t n = 0; n < ninputs; n++)
    ninput_items_required[n] = ninput_items;
}

int
ecc_streams_encode_convolutional::general_work
(int noutput_items,
 gr_vector_int &ninput_items,
 gr_vector_const_void_star &input_items,
 gr_vector_void_star &output_items)
{
  // compute the actual number of output items (1 bit char's) created.

  size_t t_n_input_items = d_encoder->compute_n_input_bits (noutput_items);
#if 1
  size_t t_n_output_items = d_encoder->compute_n_output_bits (t_n_input_items);
  assert (t_n_output_items == ((size_t)noutput_items));
#endif
  // setup the i/o buffers

  d_in_buf->set_buffer ((void**)(&input_items[0]), t_n_input_items);
  d_out_buf->set_buffer ((void**)(&output_items[0]), noutput_items);

  // "work" is handled by the encoder; which returns the actual number
  // of input items (1-bit char's) used.

  t_n_input_items = d_encoder->encode (d_in_buf, d_out_buf,
				       (size_t) noutput_items);

  assert (0);

  // consume the number of used input items on all input streams

  consume_each (t_n_input_items);

  // returns number of items written to each output stream

  return (noutput_items);
}
