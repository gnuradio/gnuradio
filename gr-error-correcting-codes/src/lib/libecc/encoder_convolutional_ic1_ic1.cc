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

#include <encoder_convolutional_ic1_ic1.h>
#include <assert.h>
#include <iostream>
#include <mld/n2bs.h>

#define DO_PRINT_DEBUG 0

static const int g_num_bits_per_byte = 8;

// FIXME
size_t
encoder_convolutional_ic1_ic1::compute_n_output_bits
(size_t n_input_bits)
{
  assert (0);
  return (0);
}

/*
 * Compute the number of input bits needed to produce
 * 'n_output' bits.  For convolutional encoders, there is
 * 1 bit output per bit input per stream, with the addition of a some
 * bits for trellis termination if selected.  Thus the input:output
 * bit ratio will be:
 * 
 * if (streaming | no termination), 1:1
 *
 * if (not streaming & termination), roughly 1:(1+X), where "X" is the
 * total memory size of the code divided by the block length in bits.
 * But this also depends on the state of the FSM ... how many bits are
 * left before termination.
 *
 * The returned value will also depend on whether bits are packed, as
 * well as whether streams are mux'ed together.
 */

size_t
encoder_convolutional_ic1_ic1::compute_n_input_bits
(size_t n_output_bits)
{
  size_t t_n_output_bits, t_n_input_bits;
  t_n_output_bits = t_n_input_bits = n_output_bits;

  if (d_do_termination == true) {

    // not streaming, doing termination; find the number of bits
    // currently available with no required inputs, if any

    size_t n_extra = 0;
    if (d_fsm_state == fsm_enc_conv_doing_term) {
      n_extra = d_total_n_delays - d_n_enc_bits;
    }

    // check to see if this is enough; return 0 if it is.

    if (n_extra >= t_n_output_bits)
      return (0);

    // remove those which require no input

    t_n_output_bits -= n_extra;

    // find the number of blocks of data which could be processed

    size_t t_n_output_bits_per_block = d_block_size_bits + d_total_n_delays;

    // get the base number of input items required for the given
    // number of blocks to be generated

    size_t t_n_blocks = t_n_output_bits / t_n_output_bits_per_block;
    t_n_input_bits = t_n_blocks * d_block_size_bits;

    // add to that the number of leftover inputs needed to generate
    // the remainder of the outputs within the remaining block, up to
    // the given block size (since anything beyond that within this
    // block requires no inputs)

    size_t t_leftover_bits = t_n_output_bits % t_n_output_bits_per_block;
    t_n_input_bits += ((t_leftover_bits > d_block_size_bits) ?
		       d_block_size_bits : t_leftover_bits);
  }

  return (t_n_input_bits);
}

void
encoder_convolutional_ic1_ic1::write_output_bits
(char** out_buf)
{
  // write all the outputs bits in d_current_outputs LSB (&1) to the
  // given output buffer.

  // one bit per output 'char' for "ic1" type output

  for (size_t n = 0; n < d_n_code_outputs; n++) {
    if (DO_PRINT_DEBUG) {
      std::cout << "Starting output_bit:\n"
	"  O_i[" << n << "][" << d_out_buf_ndx << "] = " <<
	n2bs (out_buf[n][d_out_buf_ndx], g_num_bits_per_byte) <<
	", b_out = " << n2bs (d_current_outputs[n], 1) << ", ";
    }

    out_buf[n][d_out_buf_ndx] = d_current_outputs[n];

    if (DO_PRINT_DEBUG) {
      std::cout << "O_o[][] = " <<
	n2bs (out_buf[n][d_out_buf_ndx], g_num_bits_per_byte) <<
	"\n";
    }
  }

  if (DO_PRINT_DEBUG) {
    std::cout << "Ending write_output_bits.\n";
  }

  // decrement the number of output bits left on all streams

  d_n_output_bits_left--;

  // increment the output index (not the bit shift index) for the next
  // write

  d_out_buf_ndx++;
}

void
encoder_convolutional_ic1_ic1::get_next_inputs__input
(const char** in_buf)
{
  // get the next set of input bits, moved into the LSB (&1) of
  // d_current_inputs

  // one bit per input 'char' for "ic1" type input

  for (size_t m = 0; m < d_n_code_inputs; m++) {
    d_current_inputs[m] = ((in_buf[m][d_in_buf_ndx]) & 1);

    if (DO_PRINT_DEBUG) {
      std::cout << "I[" << m << "][" << d_in_buf_ndx << "] = " <<
	n2bs (d_current_inputs[m], 1) << "\n";
    }
  }

  // decrement the number of bits left on all streams

  if (DO_PRINT_DEBUG) {
    std::cout << "# in bits left: " << d_n_input_bits_left <<
      " -> " << (d_n_input_bits_left-1) << "\n";
  }

  d_n_input_bits_left--;

  // increment the input index (not the bit shift index) for the next
  // read

  if (DO_PRINT_DEBUG) {
    std::cout << "# in buf ndx: " << d_in_buf_ndx <<
      " -> " << (d_in_buf_ndx+1) << "\n";
  }

  d_in_buf_ndx++;
}
