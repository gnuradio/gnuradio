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

#include "encoder_convolutional.h"
#include <assert.h>
#include <iostream>

#define DO_TIME_THOUGHPUT 0
#define DO_PRINT_DEBUG 1

#include <mld/mld_timer.h>
#include <mld/n2bs.h>

static const int g_max_block_size_bits = 10000000;
static const int g_max_num_streams = 10;
static const int g_num_bits_per_byte = 8;

void encoder_convolutional::encoder_convolutional_init
(int block_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int>& code_generators,
 const std::vector<int>* code_feedback,
 bool do_termination,
 int start_memory_state,
 int end_memory_state)
{
  // error checking on the input arguments is done by the trellis class

  if (code_feedback)
    d_trellis = new code_convolutional_trellis
      (block_size_bits,
       n_code_inputs,
       n_code_outputs,
       code_generators,
       *code_feedback,
       do_termination,
       end_memory_state);
  else
    d_trellis = new code_convolutional_trellis
      (block_size_bits,
       n_code_inputs,
       n_code_outputs,
       code_generators,
       do_termination,
       end_memory_state);

  // set the initial FSM state to 'init'

  d_fsm_state = fsm_enc_conv_init;

  // create the class block variables

  d_block_size_bits = block_size_bits;
  d_n_code_inputs = n_code_inputs;
  d_n_code_outputs = n_code_outputs;
  d_do_streaming = d_trellis->do_streaming ();
  d_do_termination = d_trellis->do_termination ();
  d_total_n_delays = d_trellis->total_n_delays ();

  // parse the init state

  memory_t t_mask = (memory_t)((2 << d_total_n_delays) - 1);
  size_t t_n_states = (1 << d_total_n_delays);

  if (start_memory_state & t_mask) {
    std::cout << "encoder_convolutional: Warning: " <<
      "provided end memory state out (" << end_memory_state <<
      ") is out of the state range [0, " <<
      (t_n_states-1) << "]; masking off the unused bits.\n";

    start_memory_state &= t_mask;
  }

  d_init_state = start_memory_state;

  d_current_inputs.assign (d_n_code_inputs, 0);
  d_current_outputs.assign (d_n_code_outputs, 0);
}

void
encoder_convolutional::encode_private
(const char** in_buf,
 char** out_buf)
{
  struct timeval t_tp;
  if (DO_TIME_THOUGHPUT) {
    start_timer (&t_tp);
  }

  // reset buffer indices

  d_total_n_enc_bits = d_in_buf_ndx = d_out_buf_ndx =
    d_in_bit_shift = d_out_bit_shift = 0;

  if (DO_PRINT_DEBUG) {
    std::cout << "Beginning this encode() call; starting parameters.\n";
    std::cout << "d_n_input_bits_left = " << d_n_input_bits_left << '\n';
    std::cout << "d_n_output_bits_left = " << d_n_output_bits_left << '\n';
  }

  // while there are inputs and outputs left to process ...

  while ((d_n_input_bits_left != 0) & (d_n_output_bits_left != 0)) {

    // jump to the correct state in the fsm

    switch (d_fsm_state) {

    case fsm_enc_conv_init:

      // copy the init states to the current memory

      d_memory = d_init_state;

      // if not doing streaming, things to do; else nothing more do

      if (d_do_streaming == false) {

	// reset the number of encoded bits in this block (which is
	// used to compare with the number of bits in the block)

	d_n_enc_bits = 0;
      }

      // move to the 'input' state

      d_fsm_state = fsm_enc_conv_doing_input;
      break;

    case fsm_enc_conv_doing_input:

      // working through the trellis section which requires input bits
      // from external sources; loop up to the block size (before
      // termination bits, if any), counting down the number of
      // available input bits.

      encode_loop (in_buf, out_buf, &d_n_input_bits_left, d_block_size_bits);

      // finished this loop; check for jumping to the next state

      if ((d_n_enc_bits == d_block_size_bits) & (d_do_streaming == false)) {

	// jump to another state, depending on termination requirement

	if (d_do_termination == true) {
	  d_n_enc_bits = 0;
	  d_fsm_state = fsm_enc_conv_doing_term;
	} else {
	  d_fsm_state = fsm_enc_conv_init;
	}
      }
      break;

    case fsm_enc_conv_doing_term:

      // terminating the trellis, trying to get to a specific state;
      // better get here only when do_termination is true, but check
      // just in case; lop up to the max memory, counting down the
      // number of output bits left

      if (d_do_termination == true) {
	encode_loop (in_buf, out_buf, &d_n_output_bits_left, d_total_n_delays);

	// finished this loop; check for jumping to the next state

	if (d_n_enc_bits == d_total_n_delays)
	  d_fsm_state = fsm_enc_conv_init;

      } else {
	// should never get here!
	assert (0);
      }
      break;

    default:
      // better never get here!
      assert (0);
      break;

      // done (switch) with FSM
    }

    // done (while) there are inputs and outputs
  }

  if (DO_PRINT_DEBUG) {
    std::cout << "Done with this encode() call; ending parameters.\n"
      "d_in_bit_shift = " << d_in_bit_shift << "\n"
      "d_out_bit_shift = " << d_out_bit_shift << "\n"
      "d_in_buf_ndx = " << d_in_buf_ndx << "\n"
      "d_out_buf_ndx = " << d_out_buf_ndx << "\n"
      "d_n_input_bits_left = " << d_n_input_bits_left << "\n"
      "d_n_output_bits_left = " << d_n_output_bits_left << "\n"
      "d_total_n_enc_bits = " << d_total_n_enc_bits << "\n";
  }

  if (DO_TIME_THOUGHPUT) {
    // compute the throughput for this particular function call
    u_long d_t = end_timer (&t_tp);
    std::cout << "Completed " << d_total_n_enc_bits <<
      " bits in " << d_t << " usec => " <<
      1e6*(((double) d_total_n_enc_bits)/((double) d_t)) <<
      " b/s\n";
  }
}

void
encoder_convolutional::encode_loop
(const char** in_buf,
 char** out_buf,
 size_t* which_counter,
 size_t how_many)
{
  // generic encode_loop

  if (DO_PRINT_DEBUG) {
    std::cout << "Starting encode_loop.\n";
  }

  while (((*which_counter) > 0) & (d_n_enc_bits < how_many)) {
    if (DO_PRINT_DEBUG) {
      std::cout << "*w_c = " << (*which_counter) << ", "
	"# enc_bits = " << d_n_enc_bits << " of " << how_many << ".\n"
	"Getting new inputs.\n";
    }

    // get the next set of input bits from all streams;
    // written into d_current_inputs

    get_next_inputs (in_buf);

    // use the trellis to do the encoding;
    // updates the input memory to the new memory state for the given input
    // and writes the output bits to the current_outputs

    d_trellis->encode_lookup (d_memory, d_current_inputs, d_current_outputs);

    // write the bits in d_current_outputs into the output buffer

    write_output_bits (out_buf);

    // increment the number of encoded bits for the current block, and
    // the total number of bits for this running of "encode()"

    d_n_enc_bits++;
    d_total_n_enc_bits++;
  }

  if (DO_PRINT_DEBUG) {
    std::cout << "ending encode_loop.\n";
  }
}

void
encoder_convolutional::get_next_inputs__term
()
{
  // FIXME: how to figure out which term bit to get?
  // loop to set each entry of "d_current_inputs"

  // need to do feedback separately, since it involves determining the
  // FB bit value & using that as the input value to cancel it out

  d_current_inputs.assign (d_n_code_inputs, 0);
  //   return (d_term_states[code_input_n] & 1);
}
