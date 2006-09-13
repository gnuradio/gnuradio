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

#ifndef INCLUDED_ENCODER_CONVOLUTIONAL_H
#define INCLUDED_ENCODER_CONVOLUTIONAL_H

#include "encoder.h"
#include "code_convolutional_trellis.h"

class encoder_convolutional : public encoder
{
/*!
 * class encoder_convolutional : public encoder
 *
 * Encode the incoming streams using a convolutional encoder; This is
 *     a virtual class which defines the basics of a convolutional
 *     encoder, but not how input and output bits are handled, nor
 *     feedback in the encoder.  These features are all defined by
 *     overriding methods appropriately.
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
  inline encoder_convolutional
  (int block_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generators,
   bool do_termination = true,
   int start_memory_state = 0,
   int end_memory_state = 0)
  {encoder_convolutional_init (block_size_bits,
			       n_code_inputs,
			       n_code_outputs,
			       code_generators,
			       0,
			       do_termination,
			       start_memory_state,
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

  inline encoder_convolutional
  (int block_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int>& code_generators,
   const std::vector<int>& code_feedback,
   bool do_termination = true,
   int start_memory_state = 0,
   int end_memory_state = 0)
  {encoder_convolutional_init (block_size_bits,
			       n_code_inputs,
			       n_code_outputs,
			       code_generators,
			       &code_feedback,
			       do_termination,
			       start_memory_state,
			       end_memory_state);};

  virtual ~encoder_convolutional () {delete d_trellis;};

/*
 * Compute the number of input bits needed to produce 'n_output' bits,
 * and the number of output bits which will be produced by 'n_input'
 * bits ... for a single stream only.
 *
 * For convolutional encoders, there is 1 bit output per bit input per
 * stream, with the addition of a some bits for trellis termination if
 * selected.  Thus the input:output bit ratio will be:
 * 
 * if (streaming | no termination), 1:1 exactly;
 *
 * if (not streaming & termination), depends on the state of the FSM,
 * and needs to include the number of termination bits (the total # of
 * delays); ratio is roughly (1:(1+X)), where "X" is the number of
 * termination bits divided by the (unterminated) block length in bits.
 *
 * It's up to the caller to change 'bits' to 'items' ... to know if
 * bits are packed (see e.g. code_io "ic8l") or not ("ic1l"), or all
 * streams are mux'ed together into one stream.
*/

  virtual size_t compute_n_input_bits (size_t n_output_bits);
  virtual size_t compute_n_output_bits (size_t n_input_bits);

/* for remote access to internal info */

  inline const bool do_termination () {return (d_do_termination);};
  inline const bool do_feedback () {return (d_trellis->do_feedback());};
  inline const bool do_encode_soai () {return (d_trellis->do_encode_soai());};
  inline const bool do_streaming () {return (d_do_streaming);};
  inline const size_t total_n_delays () {return (d_total_n_delays);};
  inline const size_t n_bits_to_term () {return (d_n_bits_to_term);};
  inline const code_convolutional_trellis* trellis() {return (d_trellis);};

protected:
/*
 * fsm_enc_conv_t: finite state machine for the convolutional encoder;
 *     output happens all the time, so that's built-in to each state.
 *
 * fsm_enc_conv_init: initialize for a new block / block; this is already
 *     done at instantiation, so do it only at the end of a block.
 *
 * fsm_enc_conv_doing_input: doing encoding inside the trellis
 *
 * fsm_enc_conv_doing_term: termination trellis, if requested
 */

  enum fsm_enc_conv_t {
    fsm_enc_conv_init, fsm_enc_conv_doing_input, fsm_enc_conv_doing_term
  };

  // methods defined in this class

  void encoder_convolutional_init (int block_size_bits,
				   int n_code_inputs,
				   int n_code_outputs,
				   const std::vector<int>& code_generators,
				   const std::vector<int>* code_generators,
				   bool do_termination,
				   int start_memory_state,
				   int end_memory_state);

  virtual void encode_private ();
  virtual void encode_loop (size_t& which_counter, size_t how_many);

  inline void get_next_inputs () {
    switch (d_fsm_state) {
    case fsm_enc_conv_doing_input:
      get_next_inputs__input ();
      break;
    case fsm_enc_conv_doing_term:
      get_next_inputs__term ();
      break;
    default:
      assert (0);
      break;
    }
  };

  inline virtual void get_next_inputs__input () {
    d_in_buf->read_items ((void*)(&(d_current_inputs[0])));
    d_in_buf->increment_indices ();
    d_n_bits_to_input--;
  };

  inline virtual void get_next_inputs__term () {
    d_trellis->get_termination_inputs (d_term_state,
				       d_n_enc_bits,
				       d_current_inputs);
  };

  inline virtual void write_output_bits () {
    d_out_buf->write_items ((const void*)(&(d_current_outputs[0])));
    d_out_buf->increment_indices ();
    d_n_bits_to_output--;
  };

  void get_memory_requirements (size_t m,
				size_t n,
				size_t& t_max_mem,
				size_t& t_n_unique_fb_prev_start,
				const std::vector<int>* code_feedback);

  // variables

  fsm_enc_conv_t d_fsm_state;
  bool d_do_streaming, d_do_termination;

  // "total_n_delays" is the total # of delays, needed to determine the
  // # of states in the decoder

  size_t d_total_n_delays, d_n_enc_bits;

  // the current state of the encoder (all delays / memories)

  memory_t d_memory;

  // "inputs" are the current input bits, in the LSB (&1) of each "char"

  std::vector<char> d_current_inputs;

  // "outputs" are the current output bits, in the LSB (&1) of each "char"

  std::vector<char> d_current_outputs;

  // "n_bits_to_term" is the number of bits to terminate the trellis
  // to the desired state, as determined by the termination table.
  //    d_max_delay <= d_n_bits_to_term <= d_total_n_delays
  // These numbers will vary depending on the realization.

  size_t d_n_bits_to_term;

  // "trellis" is the code trellis for the given input parameters

  code_convolutional_trellis* d_trellis;

  // "init_states" are the user-provided init states,
  // interpreted w/r.t. the actual trellis;

  memory_t d_init_state;

  // "term_state" is the ending state before termination, used by the
  // trellis to determine the correct input-bit sequences needed to
  // properly terminate the trellis to the desired end-state;
  // used only if doing termination.

  memory_t d_term_state;
};

#endif /* INCLUDED_ENCODER_CONVOLUTIONAL_H */
