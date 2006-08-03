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

#include "decoder_viterbi_full_block_i1_ic1.h"
#include <assert.h>
#include <iostream>
#include <math.h>

const int g_max_block_size_bits = 10000000;
const int g_max_num_streams = 10;
const int g_num_bits_per_byte = 8;

#define DO_TIME_THOUGHPUT 0

#define DO_PRINT_DEBUG_INST 0
#define DO_PRINT_DEBUG_FSM 0
#define DO_PRINT_DEBUG_INIT 0
#define DO_PRINT_DEBUG_UP 0
#define DO_PRINT_DEBUG_UP_0 0
#define DO_PRINT_DEBUG_UP_1 0
#define DO_PRINT_DEBUG_MIDDLE 0
#define DO_PRINT_DEBUG_MIDDLE_0 0
#define DO_PRINT_DEBUG_MIDDLE_1 0
#define DO_PRINT_DEBUG_TERM 0
#define DO_PRINT_DEBUG_TERM_1 0
#define DO_PRINT_DEBUG_OUTPUT 0
#define DO_PRINT_DEBUG_OUTPUT_0 0
#define DO_PRINT_DEBUG_EXIT 0
#define DO_PRINT_DEBUG 1

#if DO_TIME_THOUGHPUT
#include <mld/mld_timer.h>
#endif
#if DO_PRINT_DEBUG
#include <mld/n2bs.h>
#endif

// FIXME

size_t
decoder_viterbi_full_block_i1_ic1::compute_n_output_bits
(size_t n_input_items)
{
  assert (0);
  return (0);
}

/*
 * Compute the number of input items (metrics) needed to produce
 * 'noutput' bits.  For convolutional decoders, there is 1
 * bit output per metric input per stream, with the addition of a some
 * metrics for trellis termination if selected.  Without termination,
 * there is exactly 1:1 input to output (1 metric in to 1 bit out),
 * no matter the encoding type.
 *
 * if (not terminating), then get the number of output bits.
 *
 * otherwise, find the number of blocks (not necessarily an integer),
 * and then compute the number of input metrics (including termination)
 * required to produce those blocks.  Subtract the number of bits
 * leftover from the previous computation, then find the number of input
 * metrics, ceil'd to make sure there are enough.
 */

size_t
decoder_viterbi_full_block_i1_ic1::compute_n_input_metrics
(size_t n_output_bits)
{
  int t_ninput_items = 0;
  int t_noutput_bits = ((int) n_output_bits) - ((int) d_n_saved_bits);

  // if there are enough saved bits, just use those, no inputs required

  if (t_noutput_bits <= 0)
    return (0);

  // remove any bits already in the decoding trellis

  if (d_time_count != 0) {
    int t_time_bits = ((d_time_count > d_block_size_bits) ? 0 :
		       d_block_size_bits - d_time_count);
    t_noutput_bits -= t_time_bits;
    t_ninput_items += t_time_bits;
  }
  // if completing this trellis doesn't create enough outputs ...

  if (t_noutput_bits > 0) {

    // there is a 1:1 ratio between input symbols and output bits (per
    // stream), except for termination bits which are already taken
    // into account in the total # of input bits per stream class
    // variable; need to round the # output bits to the

    // find the number of blocks, ceil'd to the next higher integer

    int t_nblocks = (int) ceilf (((float) t_noutput_bits) /
				 ((float) d_block_size_bits));

    // find the number of required input bits

    t_ninput_items += t_nblocks * d_n_total_inputs_per_stream;
  }

  return (t_ninput_items);
}

// FIXME, from here down dummies to get correct compiling; for testing
// purposes only.

void
decoder_viterbi_full_block_i1_ic1::increment_input_indices
(bool while_decoding)
{
  if (while_decoding)
    std::cout << "foo!";

#if 0
// increment the in_buf index, depending on mux'ing or not
  t_in_buf_ndx += (d_do_mux_inputs == false) ? 1 : d_n_code_outputs;
#endif
}

void
decoder_viterbi_full_block_i1_ic1::increment_output_indices
(bool while_decoding)
{
  if (while_decoding)
    std::cout << "bar!";
}

void
decoder_viterbi_full_block_i1_ic1::output_bit
(char t_out_bit,
 char** out_buf,
 size_t t_output_stream)
{
  if (t_out_bit)
    std::cout << "mop!";
}
