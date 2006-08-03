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

#ifndef INCLUDED_ENCODER_H
#define INCLUDED_ENCODER_H

#include "code_types.h"

// the 'encoder' class is a virtual class upon which all encoder types
// can be built.

class encoder
{
public:
  encoder () {};
  virtual ~encoder () {};

  virtual size_t compute_n_input_bits (size_t n_output_bits) = 0;
  virtual size_t compute_n_output_bits (size_t n_input_bits) = 0;
  virtual size_t encode (const char** in_buf,
			 char** out_buf,
			 size_t n_bits_to_output);
  virtual size_t encode (const char** in_buf,
			 size_t n_bits_to_input,
			 char** out_buf);

/* for remote access to internal info */

  inline size_t block_size_bits () {return (d_block_size_bits);};
  inline size_t n_code_inputs () {return (d_n_code_inputs);};
  inline size_t n_code_outputs () {return (d_n_code_outputs);};

protected:
  /* encode_private: encode the given in_buf and write the output bits
   * to the out_buf, using internal class variables.  This function is
   * called from the publically available "encode()" methods, which
   * first set the internal class variables before executing.
   */

  virtual void encode_private (const char** in_buf, char** out_buf) = 0;

  /* inheriting methods need to figure out what makes the most sense
   * for them in terms of getting new inputs and writing outputs.
   */

  size_t d_block_size_bits, d_n_code_inputs, d_n_code_outputs;
  size_t d_n_enc_bits, d_total_n_enc_bits;
  size_t d_in_buf_ndx, d_out_buf_ndx;
  size_t d_in_bit_shift, d_out_bit_shift;
  size_t d_n_input_bits_left, d_n_output_bits_left;
};

#endif /* INCLUDED_ENCODER_H */
