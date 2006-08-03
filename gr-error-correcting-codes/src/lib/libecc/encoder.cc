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

#include <encoder.h>
#include <iostream>

#define DO_PRINT_DEBUG 0

/*
 * encode a certain number of output bits
 *
 * the 'in_buf' and 'out_buf' must have enough memory to handle the
 *     number of input and output bits; no error checking is done!
 *
 * n_bits_to_output: the number of bits per output stream to encode
 *
 * returns the actual number of bits used per input stream
 */

size_t
encoder::encode
(const char** in_buf,
 char** out_buf,
 size_t n_bits_to_output)
{
  if (DO_PRINT_DEBUG) {
    std::cout << "encode{out}(): Starting:";
  }

  // set the class-internal number of input bits and
  // output bits left to encode

  size_t saved_n_input_bits;
  saved_n_input_bits = d_n_input_bits_left =
    compute_n_input_bits (n_bits_to_output);
  d_n_output_bits_left = n_bits_to_output;

  if (DO_PRINT_DEBUG) {
    std::cout <<
      "# output bits provided = " << d_n_output_bits_left << "\n"
      "# input bits computed  = " << d_n_input_bits_left << "\n";
  }

  // call the private encode function

  encode_private (in_buf, out_buf);

  if (DO_PRINT_DEBUG) {
    std::cout <<
      "n_input_bits_used  = " <<
      (saved_n_input_bits - d_n_input_bits_left) << "\n"
      "n_output_bits_used = " <<
      (n_bits_to_output - d_n_output_bits_left) << '\n';
  }

  // return the actual number of input bits used

  return (saved_n_input_bits - d_n_input_bits_left);
}

/*
 * encode a certain number of input bits
 *
 * the 'in_buf' and 'out_buf' must have enough memory to handle the
 *     number of input and output bits; no error checking is done!
 *
 * n_bits_to_input: the number of bits per input stream to encode
 *
 * returns the actual number of bits written per output stream
 */

size_t
encoder::encode
(const char** in_buf,	
 size_t n_bits_to_input,
 char** out_buf)
{
  // set the class-internal number of input and
  // output bits left to encode

  size_t saved_n_output_bits;
  saved_n_output_bits = d_n_output_bits_left =
    compute_n_output_bits (n_bits_to_input);
  d_n_input_bits_left = n_bits_to_input;

  // call the private encode function

  encode_private (in_buf, out_buf);

  if (DO_PRINT_DEBUG) {
    std::cout << "n_input_bits_used = " <<
      (n_bits_to_input - d_n_input_bits_left) << '\n';
    std::cout << "n_output_bits_used = " <<
      (saved_n_output_bits - d_n_output_bits_left) << '\n';
  }

  // return the actual number of output bits written

  return (saved_n_output_bits - d_n_output_bits_left);
}
