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

#include <ecc_streams_encode_turbo.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <iostream>

ecc_streams_encode_turbo_sptr 
ecc_make_streams_encode_turbo
(int n_code_inputs,
 int n_code_outputs,
 const std::vector<ecc_streams_encode_convolutional_sptr> &encoders,
 const std::vector<size_t> &interleavers)
{
  return ecc_streams_encode_turbo_sptr
    (new ecc_streams_encode_turbo (n_code_inputs,
				   n_code_outputs,
				   encoders,
				   interleavers));
}

ecc_streams_encode_turbo::ecc_streams_encode_turbo
(int n_code_inputs,
 int n_code_outputs,
 const std::vector<ecc_streams_encode_convolutional_sptr> &encoders,
 const std::vector<size_t> &interleavers)
  : gr_block ("streams_encode_turbo",
	      gr_make_io_signature (0, 0, 0),
	      gr_make_io_signature (0, 0, 0))
{
  // error checking is done by the encoder class itself; just pass
  // items on here.  But ... check out individual encoders, to make
  // sure the total I/O matches those specified by the user.

  d_n_encoders = encoders.size ();

  if (d_n_encoders < 2) {
    std::cerr << "ecc_streams_encode_turbo: Error: "
      "too few (" << d_n_encoders << ") encoders specified; a Turbo "
      "code requires at least 2 constituent encoders.\n";
    assert (0);
  }

  // make sure that the block size and termination are consistent for
  // all encoders; warn the user if not, since it doesn't really
  // matter to the encoder (but it might to the decoder; remains to be
  // seen).

  encoder_convolutional* t_ec = encoders[0]->encoder ();
  d_block_size_bits = t_ec->block_size_bits ();
  d_do_termination = t_ec->do_termination ();
  bool t_diff_block_size, t_diff_termination;
  t_diff_block_size = t_diff_termination = false;

  for (size_t m = 1; m < d_n_encoders; m++) {
    t_ec = encoders[0]->encoder ();
    size_t t_block_size_bits = t_ec->block_size_bits ();
    if (t_block_size_bits != d_block_size_bits)
      t_diff_block_size = true;
    bool t_do_termination = t_ec->do_termination ();
    if (t_do_termination != d_do_termination)
      t_do_termination = true;
  }

  if (t_diff_block_size == true) {
    std::cout << "ecc_streams_encode_turbo: Warning: "
      "Some constituent encoders have different block size (bits).\n";
  }
  if (t_diff_termination == true) {
    std::cout << "ecc_streams_encode_turbo: Warning: "
      "Some constituent encoders are differently terminationed.\n";
  }

  std::cout << "ecc_streams_encode_turbo: setup:\n"
    "d_n_encoders = " << d_n_encoders << "\n"
    "n_code_inputs = " << n_code_inputs << "\n"
    "n_code_outputs = " << n_code_outputs << "\n\n"
    "Individual Encoders:\n";

  for (size_t m = 0; m < d_n_encoders; m++) {
    t_ec = encoders[m]->encoder ();
    std::cout << "  [" << (m+1) << "]:\n"
      "n_code_inputs = " << (t_ec->n_code_inputs()) << "\n"
      "n_code_outputs = " << (t_ec->n_code_outputs()) << "\n"
      "block_size_bits = " << (t_ec->block_size_bits()) << "\n"
      "do_termination = " <<
      ((t_ec->do_termination()==true)?"true":"false") << "\n";
  }

#if 1
  assert (0);
#else
  if (d_n_encoders != (interleavers.size())) {}

  d_encoder = new encoder_turbo (n_code_inputs,
				 n_code_outputs,
				 code_generators,
				 do_termination,
				 start_memory_state,
				 end_memory_state);
#endif

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

ecc_streams_encode_turbo::~ecc_streams_encode_turbo
()
{
  delete d_encoder;
  d_encoder = 0;
  delete d_in_buf;
  d_in_buf = 0;
  delete d_out_buf;
  d_out_buf = 0;
}

void ecc_streams_encode_turbo::forecast
(int noutput_items,
 gr_vector_int &ninput_items_required)
{
  int ninput_items = d_encoder->compute_n_input_bits (noutput_items);
  size_t ninputs = ninput_items_required.size();
  for (size_t n = 0; n < ninputs; n++)
    ninput_items_required[n] = ninput_items;
}

int
ecc_streams_encode_turbo::general_work
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
