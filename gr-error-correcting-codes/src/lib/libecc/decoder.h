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

#ifndef INCLUDED_DECODER_H
#define INCLUDED_DECODER_H

#include "code_io.h"

class decoder
{
  /*
   * class decoder
   * A virtual class upon which all decoder types can be built.
   * This class provides the basic methods and variables
   * generic for all decoders.
   */
public:
  decoder () {};
  virtual ~decoder () {};

  /*
   * compute_n_...: to be defined by inheriting classes, in order to
   * allow for user-functions to figure out how many inputs are
   * required to generate a given number of outputs, and vice versa.
   * Can't define them without knowing the decoder type.
   *
   * Compute the number of input items (metrics, floats, whatevers)
   * needed to produce 'n_output' bits, and the number of output bits
   * which will be produced by 'n_input' items ... for a single stream
   * only.
   */

  virtual size_t compute_n_input_items (size_t n_output_bits) = 0;
  virtual size_t compute_n_output_bits (size_t n_input_items) = 0;

  /*
   * decode: given the input and output buffers, either decode up to
   * the number of output bits or decode the number of input items
   * ... if the buffers support either decoding amounts.
   */

  virtual size_t decode (const code_input_ptr in_buf,
			 code_output_ptr out_buf,
			 size_t n_bits_to_output);
  virtual size_t decode (const code_input_ptr in_buf,
			 size_t n_items_to_input,
			 code_output_ptr out_buf);

/* for remote access to internal info */

  inline const size_t block_size_bits () {return (d_block_size_bits);};
  inline const size_t n_code_inputs () {return (d_n_code_inputs);};
  inline const size_t n_code_outputs () {return (d_n_code_outputs);};
  inline const size_t total_n_dec_bits () {return (d_total_n_dec_bits);};

protected:
  /*
   * decode_private: decode the given in_buf and write the output bits
   * to the out_buf, using internal class variables.  This function is
   * called from the publically available "encode()" methods, which
   * first set the internal class variables before executing.
   */

  virtual void decode_private () = 0;

  size_t d_block_size_bits, d_n_code_inputs, d_n_code_outputs;
  size_t d_total_n_dec_bits, d_n_items_to_input, d_n_bits_to_output;
  code_input_ptr d_in_buf;
  code_output_ptr d_out_buf;
};

#endif /* INCLUDED_DECODER_H */
