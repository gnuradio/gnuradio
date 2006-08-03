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

#ifndef INCLUDED_DECODER_H
#define INCLUDED_DECODER_H

#include "code_types.h"

// the 'decoder' class is a virtual class upon which all decoder types
// can be built.

class decoder
{
public:
  decoder () {};
  virtual ~decoder () {};

  virtual size_t compute_n_input_metrics (size_t n_output_bits) = 0;
  virtual size_t compute_n_output_bits (size_t n_input_metrics) = 0;
  virtual size_t decode (const char** in_buf,
			 char** out_buf,
			 size_t n_bits_to_output);
  virtual size_t decode (const char** in_buf,
			 size_t n_metrics_to_input,
			 char** out_buf);

protected:
  virtual void decode_private (const char** in_buf, char** out_buf) = 0;
  virtual char get_next_input (const char** in_buf, size_t code_input_n) = 0;
  virtual void output_bit (char t_out_bit, char** out_buf,
			   size_t t_output_stream) = 0;

  size_t d_block_size_bits, d_n_code_inputs, d_n_code_outputs;
  size_t d_n_dec_bits;
  size_t d_in_buf_ndx, d_out_buf_ndx;
  size_t d_in_bit_shift, d_out_bit_shift;
  size_t d_n_input_metrics_left, d_n_output_bits_left;
};

#endif /* INCLUDED_DECODER_H */
