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

#ifndef INCLUDED_ENCODER_TURBO_H
#define INCLUDED_ENCODER_TURBO_H

#include "encoder_convolutional.h"

typedef std::vector<size_t> interleaver_t;

class encoder_turbo : public encoder
{
public:
/*!
 * class encoder_turbo : public encoder
 *
 * Encode the incoming streams using a turbo encoder; This is a
 *     virtual class which defines the basics of a turbo encoder, but
 *     not how input and output bits are handled.  These features are
 *     all defined by overriding methods appropriately.
 *
 * n_code_inputs:
 * n_code_outputs: the total number of code inputs and outputs for the
 *     overall turbo encoder (not just the constituent codes).
 *
 * encoders: the constituent encoders to be used; all -should- be
 *     configured with the same "block_size" and "termination", though
 *     from this encoder's perspective it doesn't really matter.
 *
 * interleavers: the interleavers to use before each encoder,
 *     respectively, except the first encoder which will not use an
 *     interleaver.
 */

  encoder_turbo
  (int n_code_inputs,
   int n_code_outputs,
   const std::vector<encoder_convolutional*>& encoders,
   const std::vector<interleaver_t>& interleavers);

  virtual ~encoder_turbo () {};

/* for remote access to internal info */

  inline const bool do_termination () {return (d_do_termination);};

#if 1
  // dummy functions for now
  virtual size_t compute_n_input_bits (size_t n_output_bits){return(0);};
  virtual size_t compute_n_output_bits (size_t n_input_bits){return(0);};
#endif

protected:
/*
 * fsm_enc_turbo_t: finite state machine for the turbo encoder;
 *     output happens all the time, so that's built-in to each state.
 *
 * fsm_enc_turbo_init: initialize for a new frame / block; this is already
 *     done at instantiation, so do it only at the end of a block.
 *
 * fsm_enc_turbo_doing_input: doing encoding inside the trellis
 *
 * fsm_enc_turbo_doing_term: termination trellis, if requested
 */

  enum fsm_enc_turbo_t {
    fsm_enc_turbo_init, fsm_enc_turbo_doing_input, fsm_enc_turbo_doing_term
  };

  // methods defined in this class
#if 1
  // temporary just to get full compilation

  virtual void encode_private () {};

#else
  virtual void encode_private () = 0;
  virtual void encode_loop (size_t* which_counter, size_t how_many) = 0;
  virtual char get_next_bit (const char** in_buf, size_t code_input_n) = 0;
  virtual char get_next_bit__term (size_t code_input_n) = 0;

  // methods which are required by classes which inherit from this
  // one; primarily just the parts which deal with getting input bits
  // and writing output bits, changing the indices for those buffers.

  virtual char get_next_bit__input (const char** in_buf,
				    size_t code_input_n) = 0;
#endif

  // variables

  fsm_enc_turbo_t d_fsm_state;
  bool d_do_termination;
  size_t d_max_memory, d_n_memories;

  std::vector<encoder_convolutional*> d_encoders;
  std::vector<size_t> d_interleavers;
};

#endif /* INCLUDED_ENCODER_TURBO_H */
