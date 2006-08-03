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

#ifndef INCLUDED_ENCODER_CONVOLUTIONAL_IC1_IC1_H
#define INCLUDED_ENCODER_CONVOLUTIONAL_IC1_IC1_H

#include "encoder_convolutional.h"

class encoder_convolutional_ic1_ic1 : public encoder_convolutional
{
public:
/*!
 * class encoder_convolutional_ic1_ic1 : public encoder_convolutional
 *
 * Encode the incoming streams using a convolutional encoder,
 *     "feedforward" or feedback.  Optional termination, data
 *     streaming, and starting and ending memory states.
 *
 * input is "ic1": streams of char, one stream per input as defined by the
 *     instantiated code, using only the right-most justified bit as
 *     the single input bit per input item.
 *
 * output is "ic1": streams of char, one stream per output as defined by the
 *     instantiated code, using only the right-most justified bit as
 *     the single output bit per output item.
 */

  encoder_convolutional_ic1_ic1
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

  encoder_convolutional_ic1_ic1
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

  virtual ~encoder_convolutional_ic1_ic1 () {};

  virtual size_t compute_n_input_bits (size_t n_output_bits);
  virtual size_t compute_n_output_bits (size_t n_input_bits);

protected:
  virtual void get_next_inputs__input (const char** in_buf);
  virtual void write_output_bits (char** out_buf);
};

#endif /* INCLUDED_ENCODER_CONVOLUTIONAL_IC1_IC1_H */
