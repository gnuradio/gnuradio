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

#ifndef INCLUDED_DECODER_VITERBI_H
#define INCLUDED_DECODER_VITERBI_H

#include "decoder.h"
#include "encoder_convolutional.h"

class decoder_viterbi : public decoder
{
public:
/*!
 * \brief Decode the incoming metrics streams using the Viterbi algorithm.
 *
 * input: streams of metrics, 2 streams per n_code_outputs - one each for
 *     a 0- and 1-bit metric.
 *
 * output: streams of char, one stream per n_code_inputs, using only
 *     the right-most justified bit as the single bit per output item.
 *
 * sample_precision: precision of the incoming metrics
 *     if == 0, then use soft precision (32 bit float);
 *     otherwise, use an integer up to 32 bits, already sign-extended
 *     to the nearest power-of-2-sized type (char, short, long).
 *
 * l_encoder: pointer to an encoder class from which to determine the
 *     trellis transitions (states and i/o bits).
 */

  decoder_viterbi (int sample_precision,
		   encoder_convolutional* l_encoder);

  virtual ~decoder_viterbi ();

protected:
  struct state_t;

/*
 * connection_t: describes an output connection from the current
 *     time-bit memory state to the next time-bit memory state
 *
 * d_to: state pointer to which this connection going
 *
 * d_to_ndx: index of the "to" state
 *
 * d_output_bits: what are the output bits, coverted into
 *     1->+1.0, 0->-1.0, for this connection
 */

  typedef struct connection_t {
    struct state_t *d_to;
    int d_to_ndx;
    float* d_output_bits;
  } connection_t, *connection_t_ptr;

/*
 * state_t: describes a given memory state
 *
 * d_connections:  a pointer to an array of these structures
 *     will be used to describes a given time-bit's memory state;
 *     an entry will be referenced via "state_add_to", to find the
 *     connections to the next time-bit memory states.  There is
 *     one entry per each input bit combination -> 2^#I connections in all.
 *     e.g. [0] means the all 0 input;
 *     [1] means that input #1 was 1 while all the others were 0;
 *     [2] means that input #2 was 1, while all the others were 0;
 *     [3] means that inputs #1 and #2 were 1, while the others were 0.
 *
 * d_max_metric: the maximum metric thus far for this state
 *
 * d_max_state: the state from which the maximum metric was attained
 *
 * d_max_input: the input bits from which the maximum metric was attained
 */

  typedef struct state_t {
    connection_t_ptr d_connections;
    float d_max_metric;
    int d_max_state_ndx;
    int d_max_input;
  } state_t, *state_t_ptr;

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

/*
 * fsm_dec_viterbi_t: finite state machine for the Viterbi decoder
 *
 * fsm_dec_viterbi_init: initialize for a new block / block; this is
 *     already done at instantiation, so do it only at the end of a
 *     block.
 *
 * fsm_dec_viterbi_doing_up: encoding at the start of a block
 *
 * fsm_dec_viterbi_doing_middle: doing encoding inside the trellis
 *
 * fsm_dec_viterbi_doing_term: termination trellis, if requested
 */

  enum fsm_dec_viterbi_t {
    fsm_dec_viterbi_init, fsm_dec_viterbi_doing_up,
    fsm_dec_viterbi_doing_middle, fsm_dec_viterbi_doing_term
  };

  virtual void decode_private (const char** in_buf, char** out_buf);
  virtual char get_next_input (const char** in_buf, size_t code_input_n);
#if 0
  virtual void decode_loop (const char** in_buf, char** out_buf,
			    size_t* which_counter, size_t how_many);

  virtual char get_next_input__up (const char** in_buf,
				      size_t code_input_n) = 0;
  virtual char get_next_input__middle (const char** in_buf,
					  size_t code_input_n) = 0;
  virtual char get_next_input__term (size_t code_input_n) = 0;
#endif
  virtual void increment_input_indices (bool while_decoding) = 0;
  virtual void increment_output_indices (bool while_decoding) = 0;
  virtual void update_traceback__up (size_t from_state_ndx,
				     size_t to_state_ndx,
				     size_t l_input) = 0;
  virtual void update_traceback__middle () = 0;
  virtual void update_traceback__term () = 0;

  void reset_metrics (u_char which);
  void zero_metrics (u_char which);

  encoder_convolutional* d_encoder;
  fsm_dec_viterbi_t d_fsm_state;
  size_t d_max_memory, d_total_memory;
  size_t d_time_count, d_n_total_inputs_per_stream;
  size_t d_n_saved_bits, d_n_saved_bits_start_ndx, d_n_traceback_els;
  size_t d_n_states, d_n_input_combinations;
  size_t d_states_ndx, d_up_term_ndx;
  bool d_do_streaming, d_do_termination;
  std::vector<memory_t> d_init_states, d_term_states;
  char **d_save_buffer;
  state_t_ptr d_states[2];
  size_t* d_up_term_states_ndx[2];
};

#endif /* INCLUDED_DECODER_VITERBI_H */
