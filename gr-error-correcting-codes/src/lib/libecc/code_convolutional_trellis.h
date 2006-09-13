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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_CODE_CONVOLUTIONAL_TRELLIS_H
#define INCLUDED_CODE_CONVOLUTIONAL_TRELLIS_H

#include "code_types.h"
#include <vector>

/*
 * connection_t: describes an output connection from the current
 *     time-bit memory state to the next time-bit memory state
 *
 * d_to_state: memory configuration of the "to" state
 *
 * d_output_bits: the output bits for this connection, mux'ed
 */

typedef struct connection_t {
  memory_t d_to_state;
  memory_t d_output_bits;
} connection_t, *connection_t_ptr;

/*
 * trellis_t: describes a single set of trellis connections, from a
 *     time-bit to the next, forward transitions only
 *
 * This is a 2d "matrix", where the first dimention is the starting
 *     memory state, and the second is the (combined) input as an
 *     integer: e.g. for a 2 input code, if I1 = 0 and I2 = 1, then
 *     the combined input is the "number" found by appending I2 and I1
 *     together, in this case 10b = 3.
 *
 * The trellis is used to lookup information: given a starting state
 *     and inputs, return the output bits and next state.
 */

typedef std::vector<std::vector<connection_t> > trellis_t, *trellis_t_ptr;

class code_convolutional_trellis
{
/*!
 * class code_convolutional_trellis
 *
 * Create a convolutional code trellis structure, but encoding,
 * decoding, determining termination transitions, and anything else
 * which might be useful.
 *
 * block_size_bits: if == 0, then do streaming encoding ("infinite"
 *     trellis); otherwise this is the block size in bits to encode
 *     before terminating the trellis.  This value -does not- include
 *     any termination bits.
 *
 * n_code_inputs:
 * n_code_outputs:
 * code_generator: vector of integers (32 bit) representing the code
 *     to be implemented.  E.g. "4" in binary is "100", which would be
 *     "D^2" for code generation.  "6" == 110b == "D^2 + D"
 *  ==> The vector is listed in order for each output stream, so if there
 *     are 2 input streams (I1, I2) [specified in "n_code_inputs"]
 *     and 2 output streams (O1, O2) [specified in "n_code_outputs"],
 *     then the vector would be the code generator for:
 *       [I1->O1, I2->O1, I1->O2, I2->O2]
 *     with each element being an integer representation of the code.
 *     The "octal" representation is used frequently in the literature
 *     (e.g. [015, 06] == [1101, 0110] in binary) due to its close
 *     relationship with binary (each number is 3 binary digits)
 *     ... but any integer representation will suffice.
 *
 * do_termination: valid only if block_size_bits != 0, and defines
 *     whether or not to use trellis termination.  Default is to use
 *     termination when doing block coding.
 *
 * start_memory_state: when starting a new block, the starting memory
 *     state to begin encoding; there will be a helper function to
 *     assist in creating this value for a given set of inputs;
 *     default is the "all zero" state.
 * 
 * end_memory_state: when terminating a block, the ending memory
 *     state to stop encoding; there will be a helper function to
 *     assist in creating this value for a given set of inputs;
 *     default is the "all zero" state.
 */

public:
  inline code_convolutional_trellis
  (int block_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generators,
   bool do_termination = true,
   int end_memory_state = 0)
  {code_convolutional_trellis_init (block_size_bits,
				    n_code_inputs,
				    n_code_outputs,
				    code_generators,
				    NULL,
				    do_termination,
				    end_memory_state);};

/*!
 * Encoder with feedback.
 *
 * code_feedback: vector of integers (32 bit) representing the code
 *     feedback to be implemented (same as for the code_generator).
 *     For this feedback type, the LSB ("& 1") is ignored (set to "1"
 *     internally, since it's always 1) ... this (effectively)
 *     represents the input bit for the given encoder, without which
 *     there would be no encoding!  Each successive higher-order bit
 *     represents the output of that delay block; for example "6" ==
 *     110b == "D^2 + D" means use the current input bit + the output
 *     of the second delay block.  Listing order is the same as for
 *     the code_generator.
 */

  inline code_convolutional_trellis
  (int block_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generators,
   const std::vector<int> &code_feedback,
   bool do_termination = true,
   int end_memory_state = 0)
  {code_convolutional_trellis_init (block_size_bits,
				    n_code_inputs,
				    n_code_outputs,
				    code_generators,
				    &code_feedback,
				    do_termination,
				    end_memory_state);};

  virtual ~code_convolutional_trellis () {};

/* for remote access to internal info */

  inline const size_t block_size_bits () {return (d_block_size_bits);};
  inline const size_t n_code_inputs () {return (d_n_code_inputs);};
  inline const size_t n_code_outputs () {return (d_n_code_outputs);};
  inline const size_t n_states () {return (d_n_states);};
  inline const size_t n_input_combinations ()
  {return (d_n_input_combinations);};
  inline const bool do_termination () {return (d_do_termination);};
  inline const bool do_feedback () {return (d_do_feedback);};
  inline const bool do_streaming () {return (d_do_streaming);};
  inline const bool do_encode_soai () {return (d_do_encode_soai);};
  inline const size_t total_n_delays () {return (d_total_n_delays);};
  inline const size_t n_bits_to_term () {return (d_n_bits_to_term);};

  virtual char sum_bits_mod2 (memory_t in_mem, size_t max_memory);
  void get_termination_inputs (memory_t term_start_state,
			       size_t bit_num,
			       std::vector<char>& inputs) {
    // no error checking ... be careful!
    for (size_t m = 0; m < d_n_code_inputs; m++) {
      inputs[m] = ((d_term_inputs[term_start_state][m]) >> bit_num) & 1;
    }
  };

  // encode_lookup: given the starting state and inputs, return the
  // resulting state and output bits.  Two versions: the first is
  // better for decoding, while the second is better for encoding.

  void encode_lookup (memory_t& state,
		      const std::vector<char>& inputs,
		      memory_t& out_bits);
  void encode_lookup (memory_t& state,
		      const std::vector<char>& inputs,
		      std::vector<char>& out_bits);

  // methods for setting and retrieving the state, inputs, and outputs.

  void demux_state (memory_t in_state, std::vector<memory_t>& memories);
  memory_t mux_state (const std::vector<memory_t>& memories);
  void demux_inputs (memory_t inputs, std::vector<char>& in_vec);
  memory_t mux_inputs (const std::vector<char>& in_vec);
  void demux_outputs (memory_t outputs, std::vector<char>& out_vec);
  memory_t mux_outputs (const std::vector<char>& out_vec);

protected:
#if 0
/*
 * state_get_from(v,i,k): use to retrieve a given bit-memory state,
 *     from the inputs:
 *
 * memory_t v: the value from which to retrieve the given state
 * size_t i: for which input stream (0 to #I-1)
 * size_t k: the number of memory slots per input (e.g. 1+D^2 -> 2)
 */

  inline memory_t state_get_from (memory_t v,
				  size_t i,
				  size_t k)
  {return (((v)>>((i)*(k)))&((1<<(k))-1));};

/*
 * state_add_to(s,v,i,k): use to create a given bit-memory state,
 *     from the inputs:
 *
 * memory_t s: the state value to modify
 * memory_t v: value to set the state to for this input
 * size_t i: for which input stream (0 to #I-1)
 * size_t k: the number of memory slots per input (e.g. 1+D^2 -> 2)
 */

  inline void state_add_to (memory_t s,
			    memory_t v,
			    size_t i,
			    size_t k)
  {(s)|=(((v)&((1<<(k))-1))<<((i)*(k)));};
#endif

/*
 * maio(i,o): matrix access into a vector, knowing the # of code
 *     outputs (from inside the class). References into a vector with
 *     code inputs ordered by code output.
 *
 * 'i' is the 1st dimension - faster memory - the code input
 * 'o' is the 2nd dimension - slower memory - the code output
 *
 * returns ((o*n_code_inputs) + i)
 */

  inline size_t maio(size_t i, size_t o) {return ((o*d_n_code_inputs) + i);};

/*
 * maoi(i,o): matrix access into a vector, knowing the # of code
 *     inputs (from inside the class). References into a vector with
 *     code outputs ordered by code input.
 *
 * 'o' is the 1st dimension - faster memory - the code output
 * 'i' is the 2nd dimension - slower memory - the code input
 *
 * returns ((i*n_code_outputs) + o)
 */

  inline size_t maoi(size_t i, size_t o) {return ((i*d_n_code_outputs) + o);};

/*
 * max_bit_position (x): returns the bit-number of the highest "1" bit
 * in the provided value, such that the LSB would return 0 and the MSB
 * of a long would return 31.
 */

  inline size_t max_bit_position (memory_t x)
  {
    size_t t_code_mem = 0;
    memory_t t_in_code = x >> 1;
    while (t_in_code != 0) {
      t_in_code >>= 1;
      t_code_mem++;
    }

    return (t_code_mem);
  }

  // methods defined in this class

  void code_convolutional_trellis_init
  (int block_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int>& code_generators,
   const std::vector<int>* code_generators,
   bool do_termination,
   int end_memory_state);

  void create_trellis ();
  void create_termination_table (memory_t end_memory_state);
  void encode_single (memory_t in_state,
		      memory_t inputs,
		      memory_t& out_state,
		      memory_t& out_bits);
  virtual void encode_single_soai ();
  virtual void encode_single_siao ();
  virtual void encode_single_soai_fb ();
  virtual void encode_single_siao_fb ();

  void get_memory_requirements (size_t m,
				size_t n,
				size_t& t_max_mem,
				size_t& t_n_unique_fb_prev_start,
				const std::vector<int>* code_feedback);

  // variables

  size_t d_block_size_bits, d_n_code_outputs;
  size_t d_n_code_inputs, d_n_input_combinations;
  bool d_do_streaming, d_do_termination, d_do_feedback, d_do_encode_soai;

  // "max_delay" is the max # of delays for all unique generators (ff and fb), 

  size_t d_max_delay;

  // "n_memories" is the number of unique memories as determined by
  // either the feedforward or feedback generators (not both).  For
  // FF, this number equals either the number of code inputs (for
  // SIAO) or outputs (for SOAI).

  size_t d_n_memories;

  // "total_n_delays" is the total # of delays, needed to determine the
  // # of states in the decoder
  // "n_states" = (2^total_n_delays) - 1 .. the number of memory states

  size_t d_total_n_delays, d_n_states;

  // "code generators" are stored internally in "maXY(i,o)" order this
  // allows for looping over all a single output and computing all
  // input parts sequentially.

  std::vector<memory_t> d_code_generators;

  // "feedback" are found as "d_n_memories" unique entries, and stored
  // in at most 1 entry per I/O combination.  Listed in the same order
  // as "d_io_num" entries show.

  std::vector<memory_t> d_code_feedback;

  // "n_delays" is a vector, the number of delays for the FB generator
  // in the same [] location; also relates directly to the
  // "max_mem_masks" in the same [] location.

  std::vector<size_t> d_n_delays;

  // "io_num" is a vector, mapping which FB in SIAO goes with which
  // input, or which FB in SOAI goes with which output

  std::vector<size_t> d_io_num;

  // "max_mem_masks" are the memory masks, one per unique FB for SIAO;
  // otherwise not used.

  std::vector<size_t> d_states_ndx;

  // "memory" are the actual stored delay bits, one memory for each
  // unique FF or FB code generator;
  // interpreted w/r.t. the actual FF and FB code generators and
  // SOAI / SIAO realization;

  std::vector<memory_t> d_max_mem_masks;

  // "states_ndx" is a "matrix" whose contents are the indices into
  // the "io_num" vector, telling which input goes with which
  // state; uses the same "maXY(i,o)" as the code generators.

  std::vector<memory_t> d_memory;

  // "term_inputs" are the inputs required to terminate the trellis -
  //     interpreted w/r.t. the actual FF and FB code generators and
  //     SOAI / SIAO realization;
  // first dimension is the memory state #;
  // second dimension is the input stream #;
  // bits are packed, with the first input being the LSB and the last
  //     input being farthest away from the LSB.

  typedef std::vector<std::vector<memory_t> > term_input_t;
  term_input_t d_term_inputs;

  // "n_bits_to_term" is the number of bits to terminate the trellis
  // to the desired state, as determined by the termination table.
  //    d_max_delay <= d_n_bits_to_term <= d_total_n_delays
  // These numbers will vary depending on the realization.

  size_t d_n_bits_to_term;

  // "inputs" are the current input bits, in the LSB (&1) of each "char"

  std::vector<char> d_current_inputs;

  // "outputs" are the current output bits, in the LSB (&1) of each "char"

  std::vector<char> d_current_outputs;

  // "ind_outputs" are the current output bits, in the LSB (&1) of
  // each "char", for each individual memory's computations; needed
  // for soai-type feedback only

  std::vector<char> d_ind_outputs;

  // "trellis" is the single-stage memory state transition ("trellis")
  // representation for this code; forward paths only

  trellis_t d_trellis;
};

#endif /* INCLUDED_CODE_CONVOLUTIONAL_TRELLIS_H */
