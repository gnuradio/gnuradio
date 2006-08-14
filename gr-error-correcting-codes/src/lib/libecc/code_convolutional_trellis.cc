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

#include "code_convolutional_trellis.h"
#include <assert.h>
#include <iostream>

#define DO_TIME_THOUGHPUT 0
#define DO_PRINT_DEBUG 0
#define DO_PRINT_DEBUG_VARS 0
#define DO_PRINT_DEBUG_TERM 0
#define DO_PRINT_DEBUG_TERM_END 0
#define DO_PRINT_DEBUG_LOOKUP 0

#include <mld/mld_timer.h>
#include <mld/n2bs.h>

static const int g_max_block_size_bits = 10000000;
static const int g_max_num_streams = 10;
static const int g_num_bits_per_byte = 8;

/*
 * sum_bits_mod2:
 * sum the number of set bits, mod 2, for the output bit
 */

char
code_convolutional_trellis::sum_bits_mod2
(memory_t in_mem,
 size_t max_memory)
{
  // there are faster ways to do this, but this works for now; could
  // certainly do a single inline asm, which most processors provide
  // to deal with summing the bits in an integer.
  // this routine can be overridden by another method if desired.

  char t_out_bit = (char)(in_mem & 1);
  for (size_t r = max_memory; r > 0; r--) {
    in_mem >>= 1;
    t_out_bit ^= ((char)(in_mem & 1));
  }
  return (t_out_bit);
}

void
code_convolutional_trellis::code_convolutional_trellis_init
(int block_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int>& code_generators,
 const std::vector<int>* code_feedback,
 bool do_termination,
 int end_memory_state)
{
  // do error checking on the input arguments

  // make sure the block length makes sense

  if ((block_size_bits < 0) | (block_size_bits > g_max_block_size_bits)) {
    std::cerr << "code_convolutional_trellis: " <<
      "Requested block length (" << block_size_bits <<
      " bits) must be between 0 and " << g_max_block_size_bits <<
      " bits, with 0 being a streaming encoder.\n";
    assert (0);
  }

  // check to make sure the number of input streams makes sense

  if ((n_code_inputs <= 0) | (n_code_inputs > g_max_num_streams)) {
    std::cerr << "code_convolutional_trellis: " <<
      "Requested number of input streams (" <<
      n_code_inputs << ") must be between 1 and " <<
      g_max_num_streams << ".\n";
    assert (0);
  }

  // check to make sure the number of output streams makes sense

  if ((n_code_outputs <= 0) | (n_code_outputs > g_max_num_streams)) {
    std::cerr << "code_convolutional_trellis: " <<
      "Requested number of output streams (" <<
      n_code_outputs << ") must be between 1 and " <<
      g_max_num_streams << ".\n";
    assert (0);
  }

  // make sure the code_generator is the correct length

  if (code_generators.size () !=
      ((size_t)(n_code_inputs * n_code_outputs))) {
    std::cerr << "code_convolutional_trellis: " <<
      "Number of code generator entries (" << code_generators.size () <<
      ") is not equal to the product of the number of input and output" <<
      " streams (" << (n_code_inputs * n_code_outputs) << ").\n";
    assert (0);
  }

  // check for feedback (== NULL or not)

  d_do_feedback = (code_feedback != NULL);

  // create the class block variables

  d_block_size_bits = block_size_bits;
  d_n_code_inputs = n_code_inputs;
  d_n_code_outputs = n_code_outputs;
  d_do_streaming = (block_size_bits == 0);
  d_do_termination = (d_do_streaming == true) ? false : do_termination;

  if (DO_PRINT_DEBUG_VARS) {
    std::cout <<
      "d_block_size_bits = " << d_block_size_bits << "\n"
      "d_n_code_inputs   = " << d_n_code_inputs << "\n"
      "d_n_code_outputs  = " << d_n_code_outputs << "\n"
      "d_do_streaming    = " <<
      ((d_do_streaming == true) ? "true" : "false") << "\n"
      "d_do_termination  = " <<
      ((d_do_termination == true) ? "true" : "false") << "\n"
      "d_do_feedback     = " <<
      ((d_do_feedback == true) ? "true" : "false") << "\n";
  }

  // allocate the vectors for doing the encoding.  use memory_t (an
  // interger type, at least 32 bits) bits to represent memory and the
  // code, as it makes the operations quite simple the state vectors.

  // d_states is a "matrix" [#input by #outputs] containing indices
  // to memory_t's; this is done to make feedback function properly,
  // and doesn't effect the computation time for feedforward.  The
  // issue is that any code with the same feedback can use the same
  // memory - thus reducing the actual number of memories required.
  // These overlapping encoders will use the same actual memory, but
  // given that there is no way to know a-priori where they are, use
  // pointers over the full I/O matrix-space to make sure each I/O
  // encoder uses the correct memory.
  // reference the matrix using "maoi(i,o)" ... see .h file.

  // code generators (feedforward part) are [#inputs x #outputs],
  // always - one for each I/O combination.
  // reference the matrix using "maoi(i,o)" ... see .h file

  d_code_generators.assign (d_n_code_inputs * d_n_code_outputs, 0);

  // check the feedback for correctness, before anything else, since
  // any feedback impacts the total # of delays in the encoder:
  // without feedback, this is the sum of the individual delays max'ed
  // over each input (if siao) or output (if soai).

  if (d_do_feedback == true) {
    memory_t t_OR_all_feedback = 0;
    for (size_t n = 0; n < d_n_code_outputs; n++) {
      for (size_t m = 0; m < d_n_code_inputs; m++) {
	memory_t t_in_code = (*code_feedback)[maoi(m,n)];

	// OR this feedback with the overall,
	// to check for any delays used at all

	t_OR_all_feedback |= t_in_code;
      }
    }

    // check to see if all the feedback entries were either "0" or "1",
    // which implies no feedback; warn the user in that case and reset
    // the do_feedback parameter to false.

    if ((t_OR_all_feedback | 1) == 1) {
      std::cout << "code_convolutional_trellis: Warning: " <<
	"No feedback is required, ignoring feedback.\n";
      d_do_feedback = false;
    }
  }

  // copy over the FF code generators

  for (size_t n = 0; n < d_n_code_outputs; n++)
    for (size_t m = 0; m < d_n_code_inputs; m++)
      d_code_generators[maio(m,n)] = code_generators[maio(m,n)];

  // check the input FF (and FB) code generators for correctness, and
  // find the minimum memory configuration: combining via a single
  // input / all outputs (SIAO), or a single output / all inputs (SOAI).
  //
  // for FF only, look over both the SOAI and SIAO realizations to
  // find the minimum total # of delays, and use that realization
  // (SOAI is preferred if total # of delays is equal, since it's much
  // simpler to implement).
  //
  // for FB:
  //   for SIAO, check each input row (all outputs for a given input)
  //     for unique feedback; duplicate feedback entries can be
  //     combined into a single computation to reduce total # of delays.
  //   for SOAI: check each output column (all inputs for a given
  //     output) for unique feedback; duplicate feedback entries can
  //     be combined into a simgle computation (ditto).

  // check for SOAI all '0' output

  for (size_t n = 0; n < d_n_code_outputs; n++) {
    memory_t t_all_inputs_zero = 0;
    for (size_t m = 0; m < d_n_code_inputs; m++)
      t_all_inputs_zero |= d_code_generators[maio(m,n)];

    // check this input to see if all encoders were '0'; this might be
    // OK for some codes, but warn the user just in case

    if (t_all_inputs_zero == 0) {
      std::cout << "code_convolutional_trellis: Warning:"
	"Output " << n+1 << " (of " << d_n_code_outputs <<
	") will always be 0.\n";
    }
  }

  // check for SIAO all '0' input

  for (size_t m = 0; m < d_n_code_inputs; m++) {
    memory_t t_all_outputs_zero = 0;
    for (size_t n = 0; n < d_n_code_outputs; n++)
      t_all_outputs_zero |= d_code_generators[maio(m,n)];

    // check this input to see if all encoders were '0'; this might be
    // OK for some codes, but warn the user just in case

    if (t_all_outputs_zero == 0) {
      std::cout << "code_convolutional_trellis: Warning:"
	"Input " << m+1 << " (of " << d_n_code_inputs <<
	") will not be used; all encoders are '0'.\n";
    }
  }

  // check and compute memory requirements in order to determine which
  // realization uses the least memory; create and save findings to
  // not have to re-do these computations later.

  // single output, all inputs (SOAI) realization:
  // reset the global parameters

  d_code_feedback.assign (d_n_code_inputs * d_n_code_outputs, 0);
  d_n_delays.assign (d_n_code_inputs * d_n_code_outputs, 0);
  d_io_num.assign (d_n_code_inputs * d_n_code_outputs, 0);
  d_states_ndx.assign (d_n_code_inputs * d_n_code_outputs, 0);
  d_max_delay = d_total_n_delays = d_n_memories = 0;
  d_do_encode_soai = true;

  for (size_t n = 0; n < d_n_code_outputs; n++) {
    size_t t_max_mem = 0;
    size_t t_n_unique_fb_prev_start = d_n_memories;

    for (size_t m = 0; m < d_n_code_inputs; m++) {
      get_memory_requirements (m, n, t_max_mem,
			       t_n_unique_fb_prev_start, code_feedback);
      if (d_do_feedback == false) {
	d_states_ndx[maio(m,n)] = n;
      }
    }
    if (d_do_feedback == false) {
      // not feedback; just store memory requirements for this output
      d_total_n_delays += t_max_mem;
      d_n_delays[n] = t_max_mem;
      d_io_num[n] = n;
    }
  }
  if (d_do_feedback == false) {
    d_n_memories = d_n_code_outputs;
  }

  // store the parameters for SOAI

  std::vector<memory_t> t_fb_generators_soai;
  std::vector<size_t>  t_n_delays_soai, t_io_num_soai;
  std::vector<size_t> t_states_ndx_soai;
  size_t t_max_delay_soai, t_total_n_delays_soai, t_n_memories_soai;

  t_fb_generators_soai.assign (d_code_feedback.size (), 0);
  t_fb_generators_soai = d_code_feedback;
  t_n_delays_soai.assign (d_n_delays.size (), 0);
  t_n_delays_soai = d_n_delays;
  t_io_num_soai.assign (d_io_num.size (), 0);
  t_io_num_soai = d_io_num;
  t_states_ndx_soai.assign (d_states_ndx.size (), 0);
  t_states_ndx_soai = d_states_ndx;

  t_n_memories_soai = d_n_memories;
  t_total_n_delays_soai = d_total_n_delays;
  t_max_delay_soai = d_max_delay;

  // single input, all outputs (SIAO) realization
  // reset the global parameters

  d_code_feedback.assign (d_n_code_inputs * d_n_code_outputs, 0);
  d_n_delays.assign (d_n_code_inputs * d_n_code_outputs, 0);
  d_io_num.assign (d_n_code_inputs * d_n_code_outputs, 0);
  d_states_ndx.assign (d_n_code_inputs * d_n_code_outputs, 0);
  d_max_delay = d_total_n_delays = d_n_memories = 0;
  d_do_encode_soai = false;

  for (size_t m = 0; m < d_n_code_inputs; m++) {
    size_t t_max_mem = 0;
    size_t t_n_unique_fb_prev_start = d_n_memories;

    for (size_t n = 0; n < d_n_code_outputs; n++) {
      get_memory_requirements (m, n, t_max_mem,
			       t_n_unique_fb_prev_start, code_feedback);
      if (d_do_feedback == false) {
	d_states_ndx[maio(m,n)] = m;
      }
    }
    if (d_do_feedback == false) {
      // not feedback; just store memory requirements for this output
      d_total_n_delays += t_max_mem;
      d_n_delays[m] = t_max_mem;
      d_io_num[m] = m;
    }
  }
  if (d_do_feedback == false) {
    d_n_memories = d_n_code_inputs;
  }

  if (DO_PRINT_DEBUG_VARS) {
    std::cout <<
      "  t_total_n_delays_siao  = " << d_total_n_delays << "\n"
      "  t_total_n_delays_soai  = " << t_total_n_delays_soai << "\n";
  }

  // pick which realization to use;
  // siao is preferred since it's easier to debug, and more likely

  if (d_total_n_delays <= t_total_n_delays_soai) {
    // use siao
    // nothing else to do, since the global variables already hold
    // the correct values.
  } else {
    // use soai
    d_do_encode_soai = true;
    d_code_feedback = t_fb_generators_soai;
    d_n_delays = t_n_delays_soai;
    d_io_num = t_io_num_soai;
    d_states_ndx = t_states_ndx_soai;
    d_n_memories = t_n_memories_soai;
    d_total_n_delays = t_total_n_delays_soai;
    d_max_delay = t_max_delay_soai;
  }

  // make sure the block length makes sense, #2

  if ((d_do_streaming == false) & (d_block_size_bits < d_max_delay)) {
    std::cerr << "code_convolutional_trellis: " <<
      "Requested block length (" << d_block_size_bits <<
      " bit" << (d_block_size_bits > 1 ? "s" : "") <<
      ") must be at least 1 memory length (" << d_max_delay <<
      " bit" << (d_max_delay > 1 ? "s" : "") <<
      " for this code) when doing block coding.\n";
    assert (0);
  }

  // check & mask off the init states

  d_n_states = (1 << d_total_n_delays);
  d_n_input_combinations = (1 << d_n_code_inputs);

  if (DO_PRINT_DEBUG_VARS) {
    std::cout <<
      "  d_n_states             = " << d_n_states << "\n"
      "  d_n_input_combinations = " << d_n_input_combinations << "\n";
  }

  if ((d_do_feedback == true) & (d_do_encode_soai == true)) {
    // create the individual output bits used in soai feedback

    d_ind_outputs.assign (d_n_memories, 0);
  }

  // create the max_mem_mask to be used in encoding

  d_max_mem_masks.assign (d_n_memories, 0);

  for (size_t m = 0; m < d_n_memories; m++) {
    if (d_n_delays[m] == sizeof (memory_t) * g_num_bits_per_byte)
      d_max_mem_masks[m] = ((memory_t) -1);
    else
      d_max_mem_masks[m] = (memory_t)((2 << (d_n_delays[m])) - 1);
  }

  if (DO_PRINT_DEBUG_VARS) {
    std::cout <<
      "  d_n_memories      = " << d_n_memories << "\n"
      "  d_total_n_delays  = " << d_total_n_delays << " : [";
    for (size_t m = 0; m < d_n_memories; m++) {
      std::cout << d_n_delays[m];
      if (m != (d_n_memories-1))
	std::cout << ", ";
    }
    std::cout << "]\n" <<
      "  d_max_delay       = " << d_max_delay << "\n"
      "  d_do_encode_soai  = " << 
      ((d_do_encode_soai == true) ? "true" : "false") << "\n";
  }

  // zero the memories

  d_memory.assign (d_n_memories, 0);

  // create the inputs and outputs buffers

  d_current_inputs.assign (d_n_code_inputs, 0);
  d_current_outputs.assign (d_n_code_outputs, 0);

  // create the trellis for this code:

  memory_t t_mask = (memory_t)((1 << d_total_n_delays) - 1);
  memory_t t_end_memory_state = (memory_t) end_memory_state;

  if (t_end_memory_state != (t_end_memory_state & t_mask)) {
    std::cout << "code_convolutional_trellis: Warning: " <<
      "provided end memory state out (" << end_memory_state <<
      ") is out of the state range [0, " <<
      (d_n_states-1) << "]; masking off the unused bits.\n";

    end_memory_state &= t_mask;
  }

  create_trellis ();

  if (d_do_termination == true) {

    // create the termination lookup table

    create_termination_table (end_memory_state);
  }
}

void
code_convolutional_trellis::get_memory_requirements
(size_t m,   // input number
 size_t n,   // output number
 size_t& t_max_mem,
 size_t& t_n_unique_fb_prev_start,
 const std::vector<int>* code_feedback)
{
  size_t t_in_code = d_code_generators[maio(m,n)];

  // find the memory requirement for this code generator

  size_t t_code_mem_ff = max_bit_position (t_in_code);

  // check to see if this is bigger than any others in this row/column

  if (t_code_mem_ff > t_max_mem)
    t_max_mem = t_code_mem_ff;

  if (DO_PRINT_DEBUG) {
    std::cout << "c_g[" << m << "][" << n << "]{" <<
      maio(m,n) << "} = " << n2bs(t_in_code, 8) <<
      ", code_mem = " << t_code_mem_ff;
  }

  // check the feedback portion, if it exists;
  // for soai, check all the inputs which generate this output for
  // uniqueness; duplicate entries can be combined to reduce total
  // # of memories as well as required computations.

  if (d_do_feedback == true) {
    if (DO_PRINT_DEBUG) {
      std::cout << "\n";
    }

    // get the FB code; AND off the LSB for correct functionality
    // during internal computations.

    t_in_code = ((memory_t)((*code_feedback)[maio(m,n)]));
    t_in_code &= ((memory_t)(-2));

    // find the memory requirement

    size_t t_code_mem_fb = max_bit_position (t_in_code);

    if (DO_PRINT_DEBUG) {
      std::cout << "c_f[" << m << "][" << n << "]{" <<
	maio(m,n) << "} = " << n2bs(t_in_code, 8) <<
	", code_mem = " << t_code_mem_fb;
    }

    // check to see if this feedback is unique

    size_t l_n_unique_fb = t_n_unique_fb_prev_start;
    while (l_n_unique_fb < d_n_memories) {
      if (d_code_feedback[l_n_unique_fb] == t_in_code)
	break;
      l_n_unique_fb++;
    }
    if (l_n_unique_fb == d_n_memories) {

      // this is a unique feedback;

      d_code_feedback[l_n_unique_fb] = t_in_code;
      d_n_delays[l_n_unique_fb] = t_code_mem_fb;

      // increase the number of unique feedback codes

      d_n_memories++;

      // store memory requirements for this output

      if (t_max_mem < t_code_mem_fb)
	t_max_mem = t_code_mem_fb;
      d_total_n_delays += t_max_mem;

      if (DO_PRINT_DEBUG) {
	std::cout << ",  uq # " << l_n_unique_fb <<
	  ", tot_mem = " << d_total_n_delays;
      }
    } else {
      // not a unique feedback, but the FF might require more memory

      if (DO_PRINT_DEBUG) {
	std::cout << ", !uq # " << l_n_unique_fb <<
	  " = " << d_n_delays[l_n_unique_fb];
      }

      if (d_n_delays[l_n_unique_fb] < t_code_mem_ff) {
	d_total_n_delays += (t_code_mem_ff - d_n_delays[l_n_unique_fb]);
	d_n_delays[l_n_unique_fb] = t_code_mem_ff;

	if (DO_PRINT_DEBUG) {
	  std::cout << " => " << d_n_delays[l_n_unique_fb] <<
	    ", tot_mem = " << d_total_n_delays;
	}
      }
    }
    d_io_num[l_n_unique_fb] = ((d_do_encode_soai == true) ? n : m);
    d_states_ndx[maio(m,n)] = l_n_unique_fb;
  }
  if (DO_PRINT_DEBUG) {
    std::cout << "\n";
  }
  if (d_max_delay < t_max_mem)
    d_max_delay = t_max_mem;
}

void
code_convolutional_trellis::create_trellis
()
{
  // first dimension is the number of states

  d_trellis.resize (d_n_states);

  // second dimension (one per first dimension) is the number of input
  // combinations

  for (size_t m = 0; m < d_n_states; m++) {
    d_trellis[m].resize (d_n_input_combinations);
  }

  // fill in the trellis

  for (size_t m = 0; m < d_n_states; m++) {
    for (size_t n = 0; n < d_n_input_combinations; n++) {
      connection_t_ptr t_connection = &(d_trellis[m][n]);
      encode_single ((memory_t) m,
		     (memory_t) n,
		     t_connection->d_to_state,
		     t_connection->d_output_bits);
    }
  }
}

void
code_convolutional_trellis::demux_state
(memory_t in_state,
 std::vector<memory_t>& memories)
{
  // de-mux bits for the given memory state;
  // copy them into the provided vector;
  // assumes state bits start after the LSB (not at &1)

  memories.resize (d_n_memories);
  for (size_t m = 0; m < d_n_memories; m++) {
    memories[m] = (in_state << 1) & d_max_mem_masks[m];
    in_state >>= d_n_delays[m];
  }
}

memory_t
code_convolutional_trellis::mux_state
(const std::vector<memory_t>& memories)
{
  // mux bits for the given memory states in d_memory
  // assumes state bits start after the LSB (not at &1)

  memory_t t_state = 0;
  size_t shift = 0;
  for (size_t m = 0; m < d_n_memories; m++) {
    t_state |= (memories[m] >> 1) << shift;
    shift += d_n_delays[m];
  }
  return (t_state);
}

void
code_convolutional_trellis::demux_inputs
(memory_t inputs,
 std::vector<char>& in_vec)
{
  // de-mux bits for the given inputs;
  // copy them into the provided vector;

  for (size_t m = 0; m < d_n_code_inputs; m++, inputs >>= 1) {
    in_vec[m] = (char)(inputs & 1);
  }
}

memory_t
code_convolutional_trellis::mux_inputs
(const std::vector<char>& in_vec)
{
  // mux bits for the given inputs

  size_t bit_shift = 0;
  memory_t inputs = 0;
  for (size_t m = 0; m < in_vec.size(); m++, bit_shift++) {
    inputs |= (((memory_t)(in_vec[m]&1)) << bit_shift);
  }
  return (inputs);
}

void
code_convolutional_trellis::demux_outputs
(memory_t outputs,
 std::vector<char>& out_vec)
{
  // de-mux bits for the given outputs;
  // copy them into the provided vector;

  for (size_t m = 0; m < d_n_code_outputs; m++, outputs >>= 1) {
    out_vec[m] = (char)(outputs & 1);
  }
}

memory_t
code_convolutional_trellis::mux_outputs
(const std::vector<char>& out_vec)
{
  // mux bits for the given outputs

  size_t bit_shift = 0;
  memory_t outputs = 0;
  for (size_t m = 0; m < out_vec.size(); m++, bit_shift++) {
    outputs |= (((memory_t)(out_vec[m]&1)) << bit_shift);
  }
  return (outputs);
}

void
code_convolutional_trellis::encode_single
(memory_t in_state,
 memory_t inputs,
 memory_t& out_state,
 memory_t& out_bits)
{
  // set input parameters

  demux_state (in_state, d_memory);
  demux_inputs (inputs, d_current_inputs);

  // call the correct function to do the work

  if (d_do_encode_soai == true) {
    if (d_do_feedback == true) {
      encode_single_soai_fb ();
    } else {
      encode_single_soai ();
    }
  } else {
    if (d_do_feedback == true) {
      encode_single_siao_fb ();
    } else {
      encode_single_siao ();
    }
  }

  // retrieve the output parameters

  out_state = mux_state (d_memory);
  out_bits = mux_outputs (d_current_outputs);
}

void
code_convolutional_trellis::encode_lookup
(memory_t& state,
 const std::vector<char>& inputs,
 memory_t& out_bits)
{
  if (DO_PRINT_DEBUG_LOOKUP) {
    std::cout << "e_l: in_st = " << n2bs(state,d_total_n_delays) <<
      ", in = " << n2bs(mux_inputs(inputs),d_n_code_inputs);
  }

  connection_t_ptr t_connection = &(d_trellis[state][mux_inputs(inputs)]);
  state = t_connection->d_to_state;
  out_bits = t_connection->d_output_bits;

  if (DO_PRINT_DEBUG_LOOKUP) {
    std::cout << " -> out_st = " << n2bs(state,d_total_n_delays) <<
      ", out = " << n2bs(out_bits, d_n_code_outputs) << "\n";
  }
}

void
code_convolutional_trellis::encode_lookup
(memory_t& state,
 const std::vector<char>& inputs,
 std::vector<char>& out_bits)
{
  if (DO_PRINT_DEBUG_LOOKUP) {
    std::cout << "e_l: in_st = " << n2bs(state,d_total_n_delays) <<
      ", in = " << n2bs(mux_inputs(inputs),d_n_code_inputs);
  }

  connection_t_ptr t_connection = &(d_trellis[state][mux_inputs(inputs)]);
  state = t_connection->d_to_state;
  demux_outputs (t_connection->d_output_bits, out_bits);

  if (DO_PRINT_DEBUG_LOOKUP) {
    std::cout << " -> out_st = " << n2bs(state,d_total_n_delays) <<
      ", out = " << n2bs(t_connection->d_output_bits,
			 d_n_code_outputs) << "\n";
  }
}

void
code_convolutional_trellis::create_termination_table
(memory_t end_memory_state)
{
  // somewhat involved, but basically start with the terminating state
  // and work backwards d_total_n_delays, then create a
  // std::vector<memory_t> of length n_states, once per path required
  // to get from the given state to the desired termination state.
  //
  // each entry represents the bits required to terminate that
  // particular state, listed in order from LSB for the first input
  // bit to the MSB for the last input bit.

  // create a reverse trellis
  // it's temporary, just for doing the termination, so just do it locally

  trellis_t t_trellis;

  // first dimension is the number of states

  t_trellis.resize (d_n_states);

  // second dimension (one per first dimension) is the number of input
  // combinations; reserve so that the size() can be used for adding new

  for (size_t m = 0; m < d_n_states; m++) {
    t_trellis[m].reserve (d_n_input_combinations);
  }

  std::vector<memory_t> tmp(d_n_memories);
  demux_state (end_memory_state, tmp);

  memory_t to_state, outputs;
  connection_t t_conn;

  // fill in the trellis; discard the outputs
  // set the trellis node's output bits to the input

  for (size_t m = 0; m < d_n_states; m++) {
    for (size_t n = 0; n < d_n_input_combinations; n++) {
      to_state = outputs = 0;

      encode_single ((memory_t) m,
		     (memory_t) n,
		     to_state,
		     outputs);

      t_conn.d_to_state = (memory_t) m;
      t_conn.d_output_bits = (memory_t) n;
      t_trellis[to_state].push_back (t_conn);

      if (DO_PRINT_DEBUG_TERM) {
	std::cout << "[" << n2bs(m,d_total_n_delays) << "][" <<
	  n2bs(n,d_n_code_inputs) << "] -> " <<
	  n2bs(to_state, d_total_n_delays) << "\n";
      }
    }
  }

  if (DO_PRINT_DEBUG_TERM) {
    std::cout << "Trellis:\n";

    for (size_t m = 0; m < d_n_states; m++) {
      for (size_t n = 0; n < d_n_input_combinations; n++) {
	std::cout << "[" << n2bs(t_trellis[m][n].d_to_state,
				 d_total_n_delays) << "] : [" <<
	  n2bs(t_trellis[m][n].d_output_bits, d_n_code_inputs) << "] -> " <<
	  n2bs(m, d_total_n_delays) << "\n";
      }
    }
  }

  // need 2 of most buffers: one for the current-in-use variables, and
  // one for the to-be-determined variables

  // create the term input bit vectors

  term_input_t t_term_inputs[2];
  t_term_inputs[0].resize (d_n_states);
  for (size_t m = 0; m < d_n_states; m++) {
    t_term_inputs[0][m].assign (d_n_code_inputs, 0);
  }
  t_term_inputs[1].resize (d_n_states);
  for (size_t m = 0; m < d_n_states; m++) {
    t_term_inputs[1][m].assign (d_n_code_inputs, 0);
  }

  // create the list of "in-use" states for the current t_term_inputs

  std::vector<size_t> t_used_states_ndx[2];
  t_used_states_ndx[0].assign (d_n_states, 0);
  t_used_states_ndx[1].assign (d_n_states, 0);

  std::vector<bool> t_in_use_states[2];
  t_in_use_states[0].assign (d_n_states, false);
  t_in_use_states[1].assign (d_n_states, false);

  // termporary 'inputs' place holder, in order to use the class's
  // built-in inputs demux'er.

  std::vector<char> t_inputs;
  t_inputs.assign (d_n_code_inputs, 0);

  // setup the first state

  size_t t_which_input = 0;
  t_used_states_ndx[t_which_input][0] = (size_t) end_memory_state;
  t_in_use_states[t_which_input][(size_t) end_memory_state] = true;
  size_t n_states_used[2];
  n_states_used[t_which_input] = 1;
  n_states_used[t_which_input^1] = 0;

  // loop until either the number of states has been reached, or the
  // number of input term bits (per stream) is too large (in which
  // case this code can't be terminated ... shouldn't happen, but it's
  // here just in case.

  size_t n_input_term_bits = 0;

  while ((n_states_used[t_which_input] < d_n_states) &
	 (n_input_term_bits < 2*d_total_n_delays)) {

    if (DO_PRINT_DEBUG_TERM) {
      std::cout << "Starting loop:\n# states in use = " <<
	n_states_used[t_which_input] << " (of " << d_n_states <<
	"), # term bits = " << n_input_term_bits << " (of between " <<
	d_total_n_delays << " and " << (2*d_total_n_delays) << ")\n";
    }

    // loop over all current in-use states

    for (size_t m = 0; m < n_states_used[t_which_input]; m++) {

      // get the current state to work with

      size_t t_state_ndx = t_used_states_ndx[t_which_input][m];

      for (size_t p = 0; p < d_n_input_combinations; p++) {
	memory_t t_from_state = t_trellis[t_state_ndx][p].d_to_state;
	if (t_in_use_states[t_which_input^1][t_from_state] == false) {
	  // not currently in use; make use of it
	  // if it's already in use, then why duplicate the inputs?

	  memory_t t_input = t_trellis[t_state_ndx][p].d_output_bits;

	  if (DO_PRINT_DEBUG_TERM) {
	    std::cout << "doing st[" << n2bs(t_state_ndx,d_total_n_delays) <<
	      "] <- [" << n2bs(t_from_state,d_total_n_delays) << "]: in = " <<
	      n2bs(t_input, d_n_code_inputs) << "\n";
	  }

	  // copy over the current state's input bits to the 'from'
	  // state's input bits, in the "current" term inputs

	  t_term_inputs[t_which_input^1][t_from_state] =
	    t_term_inputs[t_which_input][t_state_ndx];

	  // update the copied bits with the current inputs, in the
	  // correct bit position: LSB (&1) -> first input, LSB+1 (&2)
	  // -> second input, etc...

	  demux_inputs (t_input, t_inputs);

	  for (size_t n = 0; n < d_n_code_inputs; n++) {
	    memory_t t_term = t_term_inputs[t_which_input^1][t_from_state][n];
	    t_term <<= 1;
	    t_term |= ((memory_t)(t_inputs[n] & 1));
	    t_term_inputs[t_which_input^1][t_from_state][n] = t_term;
	  }

	  // add this from state to the list of states for the next run

	  t_used_states_ndx[t_which_input^1][n_states_used[t_which_input^1]] =
	    t_from_state;

	  // and set that this state is in use

	  t_in_use_states[t_which_input^1][t_from_state] = true;

	  // increase the number of next used states

	  n_states_used[t_which_input^1] += 1;
	}
      }
    }

    // update / reset variables for this run-through

    // swap buffers ("^1" is always the next set of buffers)

    t_which_input ^= 1;

    // zero the next # of states used

    n_states_used[t_which_input^1] = 0;

    // reset the next 'in use' buffer

    t_in_use_states[t_which_input^1].assign (d_n_states, false);

    // increase the number of required term bits (per stream)

    n_input_term_bits++;
  }

  if (n_states_used[t_which_input] != d_n_states) {
    std::cerr << "code_convolutional_trellis::create_termination_table: "
      "Warning: Unable to determine all required termination inputs for the "
      "provided termination state.  Turning termination off.\n";
    d_do_termination = false;
  } else {
    d_n_bits_to_term = n_input_term_bits;

    d_term_inputs.resize (d_n_states);
    for (size_t m = 0; m < d_n_states; m++) {
      d_term_inputs[m].assign (d_n_code_inputs, 0);
    }

    d_term_inputs = t_term_inputs[t_which_input];

    if (DO_PRINT_DEBUG_TERM_END) {
      std::cout << "# Term inputs / stream = " << d_n_bits_to_term << "\n";

      for (size_t m = 0; m < d_n_states; m++) {
	for (size_t n = 0; n < d_n_code_inputs; n++) {
	  std::cout << " [" << n2bs(m,d_total_n_delays) << "][" << n <<
	    "] = " << n2bs(d_term_inputs[m][n], d_n_bits_to_term) << "\n";
	}
      }
    }
  }
}

void
code_convolutional_trellis::encode_single_soai
()
{
  // single-output, all inputs; no feedback

  if (DO_PRINT_DEBUG) {
    std::cout << "Starting encode_single_soai.\n";
  }

  // shift memories down by 1 bit to make room for feedback; no
  // masking required.

  for (size_t p = 0; p < d_n_memories; p++) {
    if (DO_PRINT_DEBUG) {
      std::cout << "m_i[" << p << "] = " <<
	n2bs(d_memory[p], 1+d_n_delays[p]);
    }

    d_memory[p] >>= 1;

    if (DO_PRINT_DEBUG) {
      std::cout << "  >>= 1 -> " <<
	n2bs(d_memory[p], 1+d_n_delays[p]) << "\n";
    }
  }

  // for each input bit, if that bit's a '1', then XOR the code
  // generators into the correct state's memory.

  for (size_t m = 0; m < d_n_code_inputs; m++) {
    if (DO_PRINT_DEBUG) {
      std::cout << "c_i[" << m << "] = " <<
	n2bs(d_current_inputs[m],1);
    }
    if (d_current_inputs[m] == 1) {
      if (DO_PRINT_DEBUG) {
	std::cout << "\n";
      }
      for (size_t n = 0; n < d_n_code_outputs; n++) {
	if (DO_PRINT_DEBUG) {
	  std::cout << "m_i[s_ndx[" << m << "][" << n << "] == " <<
	    d_states_ndx[maio(m,n)] << "] = " <<
	    n2bs(d_memory[d_states_ndx[maio(m,n)]],
		 1+d_n_delays[d_states_ndx[maio(m,n)]]);
	}

	d_memory[d_states_ndx[maio(m,n)]] ^= d_code_generators[maio(m,n)];

	if (DO_PRINT_DEBUG) {
	  std::cout << " ^= c_g[][] == " <<
	    n2bs(d_code_generators[maio(m,n)],
		 1+d_n_delays[d_states_ndx[maio(m,n)]]) <<
	    " -> " << n2bs(d_memory[d_states_ndx[maio(m,n)]],
			   1+d_n_delays[d_states_ndx[maio(m,n)]]) << "\n";
	}
      }
    } else if (DO_PRINT_DEBUG) {
      std::cout << " ... nothing to do\n";
    }
  }

  for (size_t p = 0; p < d_n_code_outputs; p++) {
    d_current_outputs[p] = 0;
  }

  // create the output bits, by XOR'ing the individual unique
  // memory(ies) into the correct output bit

  for (size_t p = 0; p < d_n_memories; p++) {
    d_current_outputs[d_io_num[p]] ^= ((char)(d_memory[p] & 1));
  }

  if (DO_PRINT_DEBUG) {
    std::cout << "ending encode_single_soai.\n";
  }
}

void
code_convolutional_trellis::encode_single_soai_fb
()
{
  // single-output, all inputs; with feedback

  if (DO_PRINT_DEBUG) {
    std::cout << "Starting encode_single_soai_fb.\n";
  }

  // shift memories down by 1 bit to make room for feedback; no
  // masking required.

  for (size_t p = 0; p < d_n_memories; p++) {
    if (DO_PRINT_DEBUG) {
      std::cout << "m_i[" << p << "] = " <<
	n2bs(d_memory[p], 1+d_n_delays[p]);
    }

    d_memory[p] >>= 1;

    if (DO_PRINT_DEBUG) {
      std::cout << " >>= 1 -> " <<
	n2bs(d_memory[p], 1+d_n_delays[p]) << "\n";
    }
  }

  // for each input bit, if that bit's a '1', then XOR the code
  // generators into the correct state's memory.

  for (size_t m = 0; m < d_n_code_inputs; m++) {
    if (DO_PRINT_DEBUG) {
      std::cout << "in[" << m << "] = " << ((int)d_current_inputs[m]) << "\n";
    }

    if (d_current_inputs[m] == 1) {
      for (size_t n = 0; n < d_n_code_outputs; n++) {
	if (DO_PRINT_DEBUG) {
	  size_t p = d_states_ndx[maio(m,n)];
	  std::cout << "d_m[" << p << "] = " <<
	    n2bs(d_memory[p], 1+d_n_delays[p]) <<
	    " ^= c_g[" << maio(m,n) << "] = " <<
	    n2bs(d_code_generators[maio(m,n)],1+d_n_delays[p]);
	}

	d_memory[d_states_ndx[maio(m,n)]] ^= d_code_generators[maio(m,n)];

	if (DO_PRINT_DEBUG) {
	  size_t p = d_states_ndx[maio(m,n)];
	  std::cout << " -> " << n2bs(d_memory[p],1+d_n_delays[p]) << "\n";
	}
      }
    }
  }

  for (size_t p = 0; p < d_n_code_outputs; p++) {
    d_current_outputs[p] = 0;
  }

  // create the output bits, by XOR'ing the individual unique
  // memory(ies) into the correct output bit

  for (size_t p = 0; p < d_n_memories; p++) {
    if (DO_PRINT_DEBUG) {
      std::cout << "c_o[" << d_io_num[p] << "] = " <<
	n2bs(d_current_outputs[d_io_num[p]],1) << " ^= m[" <<
	p << "]&1 = " << n2bs(d_memory[p],1);
    }

    d_ind_outputs[p] = ((char)(d_memory[p] & 1));
    d_current_outputs[d_io_num[p]] ^= d_ind_outputs[p];

    if (DO_PRINT_DEBUG) {
      std::cout << " -> " << n2bs(d_current_outputs[d_io_num[p]],1) << "\n";
    }
  }

  // now that the output bits are fully created, XOR the FB back
  // into the memories; the feedback bits have the LSB (&1) masked
  // off already so that it doesn't contribute.

  for (size_t p = 0; p < d_n_memories; p++) {
    if (DO_PRINT_DEBUG) {
      std::cout << "i_o[" << p << "] = " <<
	n2bs(d_ind_outputs[p],1) << ", m[" << p << "] = " <<
	n2bs(d_memory[p],1+d_n_delays[p]);
    }

    if (d_ind_outputs[p] == 1) {
      d_memory[p] ^= d_code_feedback[p];

      if (DO_PRINT_DEBUG) {
	std::cout << " ^= c_f[" << p << "] = " <<
	  n2bs(d_code_feedback[p],1+d_n_delays[p]) <<
	  " -> " << n2bs(d_memory[p],1+d_n_delays[p]);
      }
    }
    if (DO_PRINT_DEBUG) {
      std::cout << "\n";
    }
  }

  if (DO_PRINT_DEBUG) {
    std::cout << "ending encode_single_soai_fb.\n";
  }
}

void
code_convolutional_trellis::encode_single_siao
()
{
  // single input, all outputs; no feedback

  if (DO_PRINT_DEBUG) {
    std::cout << "starting encode_single_siao.\n";
  }

  // update the memories with the current input bits;

  // for each unique memory (1 per input), shift the delays and mask
  // off the extra high bits; then XOR in the input bit.

  for (size_t p = 0; p < d_n_memories; p++) {
    d_memory[p] |= ((memory_t)(d_current_inputs[d_io_num[p]]));
  }

  // create the output bits: for each output, loop over all inputs,
  // find the output bits for each encoder, and XOR each together
  // then sum (would usually be sum then XOR, but they're mutable in
  // base-2 and it's faster this way).

  for (size_t n = 0; n < d_n_code_outputs; n++) {
    memory_t t_mem = 0;
    for (size_t m = 0; m < d_n_code_inputs; m++) {
      t_mem ^= ((d_memory[d_states_ndx[maio(m,n)]]) &
		d_code_generators[maio(m,n)]);
    }
    d_current_outputs[n] = sum_bits_mod2 (t_mem, d_max_delay);
  }

  // post-shift & mask the memory to guarantee that output
  // state is in the correct bit positions (1 up, not the LSB)

  for (size_t p = 0; p < d_n_memories; p++) {
    d_memory[p] = (d_memory[p] << 1) & d_max_mem_masks[p];
  }

  if (DO_PRINT_DEBUG) {
    std::cout << "ending encode_single_siao.\n";
  }
}

void
code_convolutional_trellis::encode_single_siao_fb
()
{
  // single input, all outputs; with feedback

  if (DO_PRINT_DEBUG) {
    std::cout << "starting encode_single_siao_fb.\n";
  }

  // update the memories with the current input bits;

  // for each unique memory (1 per input), shift the delays and mask
  // off the extra high bits; then XOR in the input bit.
  // with FB: find the feedback bit, and OR it into the input bit's slot;

  for (size_t p = 0; p < d_n_memories; p++) {
    memory_t t_mem = d_memory[p];
    memory_t t_fb = t_mem & d_code_feedback[p];
    char t_fb_bit = sum_bits_mod2 (t_fb, d_max_delay);
    t_mem |= ((memory_t) t_fb_bit);
    d_memory[p] = t_mem ^ ((memory_t)(d_current_inputs[d_io_num[p]]));
  }

  // create the output bits: for each output, loop over all inputs,
  // find the output bits for each encoder, and XOR each together
  // then sum (would usually be sum then XOR, but they're mutable in
  // base-2 and it's faster this way).

  for (size_t n = 0; n < d_n_code_outputs; n++) {
    memory_t t_mem = 0;
    for (size_t m = 0; m < d_n_code_inputs; m++) {
      t_mem ^= ((d_memory[d_states_ndx[maio(m,n)]]) &
		d_code_generators[maio(m,n)]);
    }
    d_current_outputs[n] = sum_bits_mod2 (t_mem, d_max_delay);
  }

  // post-shift & mask the memory to guarantee that output
  // state is in the correct bit positions (1 up, not the LSB)

  for (size_t p = 0; p < d_n_memories; p++) {
    d_memory[p] = (d_memory[p] << 1) & d_max_mem_masks[p];
  }

  if (DO_PRINT_DEBUG) {
    std::cout << "ending encode_loop_siao_fb.\n";
  }
}
