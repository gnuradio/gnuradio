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
#define DO_PRINT_DEBUG_FSM 0

#include <mld/mld_timer.h>

void
encoder_convolutional::encoder_convolutional_init
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
  d_n_enc_bits = 0;

  // create the class block variables

  d_block_size_bits = block_size_bits;
  d_n_code_inputs = n_code_inputs;
  d_n_code_outputs = n_code_outputs;
  d_do_streaming = d_trellis->do_streaming ();
  d_do_termination = d_trellis->do_termination ();
  d_total_n_delays = d_trellis->total_n_delays ();
  d_n_bits_to_term = d_trellis->n_bits_to_term ();

  // parse the init state

  memory_t t_mask = (memory_t)((1 << d_total_n_delays) - 1);
  size_t t_n_states = (1 << d_total_n_delays);
  memory_t t_start_memory_state = (memory_t) start_memory_state;

  if (t_start_memory_state != (t_start_memory_state & t_mask)) {
    std::cout << "encoder_convolutional: Warning: " <<
      "provided start memory state (" << start_memory_state <<
      ") is out of the state range [0, " <<
      (t_n_states-1) << "]; masking off the unused bits.\n";

    start_memory_state &= t_mask;
  }

  d_init_state = start_memory_state;

  // reset the inputs and outputs, both to get the correct size() and
  // for the sake of zeroing them out.

  d_current_inputs.assign (d_n_code_inputs, 0);
  d_current_outputs.assign (d_n_code_outputs, 0);
}

void
encoder_convolutional::encode_private
()
{
  struct timeval t_tp;
  if (DO_TIME_THOUGHPUT) {
    start_timer (&t_tp);
  }

  // reset buffer indices

  d_total_n_enc_bits = 0;

  // while there are inputs and outputs left to process ...

  while ((d_n_bits_to_input != 0) |
	 (d_n_bits_to_output != 0) |
	 (d_fsm_state == fsm_enc_conv_init)) {

    // jump to the correct state in the fsm

    switch (d_fsm_state) {

    case fsm_enc_conv_init:

      if (DO_PRINT_DEBUG_FSM)
	std::cout << "Starting FSM Doing Init\n";

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

      if (DO_PRINT_DEBUG_FSM)
	std::cout << "FSM State set to Doing Input\n"
	  "Ending FSM Doing Init\n";

      break;

    case fsm_enc_conv_doing_input:

      if (DO_PRINT_DEBUG_FSM)
	std::cout << "Starting FSM Doing Input\n";

      // working through the trellis section which requires input bits
      // from external sources; loop up to the block size (before
      // termination bits, if any), counting down the number of
      // available input bits.

      encode_loop (d_n_bits_to_input, d_block_size_bits);

      // finished this loop; check for jumping to the next state

      if ((d_n_enc_bits == d_block_size_bits) & (d_do_streaming == false)) {

	// jump to another state, depending on termination requirement

	if (d_do_termination == true) {
	  d_n_enc_bits = 0;
	  d_fsm_state = fsm_enc_conv_doing_term;

	  if (DO_PRINT_DEBUG_FSM)
	    std::cout << "FSM State set to Doing Term\n";

	} else {
	  d_fsm_state = fsm_enc_conv_init;

	  if (DO_PRINT_DEBUG_FSM)
	    std::cout << "FSM State set to Init\n";
	}
      }

      if (DO_PRINT_DEBUG_FSM)
	std::cout << "Ending FSM Doing Input\n";

      break;

    case fsm_enc_conv_doing_term:

      if (DO_PRINT_DEBUG_FSM)
	std::cout << "Starting FSM Doing Term\n";

      // terminating the trellis, trying to get to a specific state;
      // better get here only when do_termination is true, but check
      // just in case; lop up to the max memory, counting down the
      // number of output bits left

      if (d_do_termination == true) {
	if (d_n_enc_bits == 0) {
	  // first time through this; save the starting termination state
	  d_term_state = d_memory;
	}

	encode_loop (d_n_bits_to_output, d_n_bits_to_term);

	// finished this loop; check for jumping to the next state

	if (d_n_enc_bits == d_n_bits_to_term) {
	  d_fsm_state = fsm_enc_conv_init;

	  if (DO_PRINT_DEBUG_FSM)
	    std::cout << "FSM State set to Init\n";
	}
      } else {
	// should never get here!
	assert (0);
      }

      if (DO_PRINT_DEBUG_FSM)
	std::cout << "Ending FSM Doing Term\n";

      break;

    default:
      // better never get here!
      assert (0);
      break;

      // done (switch) with FSM
    }

    // done (while) there are inputs and outputs
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
(size_t& which_counter,
 size_t how_many)
{
  // generic encode_loop

  while ((which_counter > 0) & (d_n_enc_bits < how_many)) {

    // get the next set of input bits from all streams;
    // written into d_current_inputs

    get_next_inputs ();

    // use the trellis to do the encoding;
    // updates the input memory to the new memory state for the given input
    // and writes the output bits to the current_outputs

    d_trellis->encode_lookup (d_memory, d_current_inputs, d_current_outputs);

    // write the bits in d_current_outputs into the output buffer

    write_output_bits ();

    // increment the number of encoded bits for the current block, and
    // the total number of bits for this running of "encode()"

    d_n_enc_bits++;
    d_total_n_enc_bits++;
  }
}

size_t
encoder_convolutional::compute_n_output_bits
(size_t n_input_bits)
{
  size_t t_n_output_bits, t_n_input_bits;
  t_n_output_bits = t_n_input_bits = n_input_bits;

  if (d_do_termination == true) {
    // not streaming, doing termination; find the number of bits
    // currently available with no required inputs, if any

    size_t t_n_blocks, t_n_extra;
    t_n_blocks = t_n_extra = 0;

    if (d_fsm_state == fsm_enc_conv_doing_term) {

      t_n_extra = d_n_bits_to_term - d_n_enc_bits;
      t_n_blocks = t_n_input_bits / d_block_size_bits;

    } else {
      // find the number of blocks which will be completed by the
      // additional inputs

      t_n_blocks = (d_n_enc_bits + t_n_input_bits) / d_block_size_bits;
    }

    // add the number of term_bits*blocks to the number of output bits

    t_n_output_bits += (t_n_blocks * d_n_bits_to_term);
    t_n_output_bits += t_n_extra;
  }

  return (t_n_output_bits);
}

size_t
encoder_convolutional::compute_n_input_bits
(size_t n_output_bits)
{
  size_t t_n_output_bits, t_n_input_bits;
  t_n_output_bits = t_n_input_bits = n_output_bits;

  if (d_do_termination == true) {

    // not streaming, doing termination; find the number of bits
    // currently available with no required inputs, if any

    size_t n_extra = 0;
    if (d_fsm_state == fsm_enc_conv_doing_term) {
      n_extra = d_n_bits_to_term - d_n_enc_bits;
    }

    // check to see if this is enough; return 0 if it is.

    if (n_extra >= t_n_output_bits)
      return (0);

    // remove those which require no input

    t_n_output_bits -= n_extra;
    t_n_input_bits -= n_extra;

    // add into the output bits the number of currently encoded input bits

    if (d_fsm_state == fsm_enc_conv_doing_input)
      t_n_output_bits += d_n_enc_bits;

    // find the number of blocks of data which will create the
    // remaining number of output bits, though possibly not the
    // termination bits of a final block (done later)

    size_t t_n_output_bits_per_block = d_block_size_bits + d_n_bits_to_term;
    size_t t_n_blocks = t_n_output_bits / t_n_output_bits_per_block;

    // remove the termination bits from the remaining # of input bits

    t_n_input_bits -= (t_n_blocks * d_n_bits_to_term);

    // do the remaining inputs / outputs create at least 1
    // block_size_bits (but less than a full block + termination bits)

    t_n_output_bits -= (t_n_blocks * t_n_output_bits_per_block);

    if (t_n_output_bits >= d_block_size_bits) {
      n_extra = t_n_output_bits - d_block_size_bits;
      t_n_input_bits -= n_extra;
    }
  }

  return (t_n_input_bits);
}
