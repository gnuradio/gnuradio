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

#ifndef INCLUDED_ENCODER_CONVOLUTIONAL_IC8_IC8_H
#define INCLUDED_ENCODER_CONVOLUTIONAL_IC8_IC8_H

#include "encoder_convolutional.h"

/*!
 * class encoder_convolutional_ic8_ic8 : public encoder_convolutional
 *
 * Encode the incoming streams using a convolutional encoder,
 *     "feedforward" or feedback.  Optional termination, data
 *     streaming, and starting and ending memory states.
 *
 * input is "ic8": streams of char, one stream per input as defined by the
 *     instantiated code, using all 8 bits in the char.
 *
 * FIXME: need to have inputs for MSB or LSB first input and output.
 *
 * output is "ic8": streams of char, one stream per output as defined by the
 *     instantiated code, using all 8 bits in the char.
 */

class encoder_convolutional_ic8_ic8 : public encoder_convolutional
{
public:
  encoder_convolutional_ic8_ic8
  (int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generators,
   bool do_termination = true,
   int start_memory_state = 0,
   int end_memory_state = 0)
    : encoder_convolutional (frame_size_bits,
			     n_code_inputs,
			     n_code_outputs,
			     code_generators,
			     do_termination,
			     start_memory_state,
			     end_memory_state) {};

  encoder_convolutional_ic8_ic8
  (int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generators,
   const std::vector<int> &code_feedback,
   bool do_termination = true,
   int start_memory_state = 0,
   int end_memory_state = 0)
    : encoder_convolutional (frame_size_bits,
			     n_code_inputs,
			     n_code_outputs,
			     code_generators,
			     code_feedback,
			     do_termination,
			     start_memory_state,
			     end_memory_state) {};

  virtual ~encoder_convolutional_ic8_ic8 () {};

  virtual size_t compute_n_input_bits (size_t n_output_bits);
  virtual size_t compute_n_output_bits (size_t n_input_bits);

protected:
  virtual char get_next_bit__input (const char** in_buf,
				    size_t code_input_n);
  virtual char get_next_bit__term (size_t code_input_n);
  virtual void output_bit (char t_out_bit, char** out_buf,
			   size_t t_output_stream);
  virtual void increment_io_indices (bool while_encoding);
  virtual void update_memory_post_encode ();
};

#endif /* INCLUDED_ENCODER_CONVOLUTIONAL_IC8_IC8_H */
