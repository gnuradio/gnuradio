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
(const code_input_ptr in_buf,
 code_output_ptr out_buf,
 size_t n_bits_to_output)
{
  if (in_buf == 0) {
    std::cerr << "encoder::encode{output}: Error: "
      "input buffer is NULL.\n";
    assert (0);
  }
  if (out_buf == 0) {
    std::cerr << "encoder::encode{output}: Error: "
      "output buffer is NULL.\n";
    assert (0);
  }
  if (n_bits_to_output == 0) {
    std::cerr << "encoder::encode{output}: Warning: "
      "no output bits requested.\n";
    return (0);
  }

  // set the class-internal number of input bits and
  // output bits left to encode

  d_in_buf = in_buf;
  d_out_buf = out_buf;

  // check that there are enough output buffer items

  if (d_out_buf->n_items_left() < n_bits_to_output) {
    std::cerr << "encoder::encode{output}: Warning: "
      "output buffer size (" << d_out_buf->n_items_left() <<
      ") is less than the desired number of output items (" <<
      n_bits_to_output << ") ... using lower number.\n";
    n_bits_to_output = d_out_buf->n_items_left();
  }

  // check that there are enough input buffer items

  size_t n_bits_to_input = compute_n_input_bits (n_bits_to_output);

  if (d_in_buf->n_items_left() < n_bits_to_input) {
    std::cerr << "encoder::encode{output}: Warning: input buffer size (" <<
      d_in_buf->n_items_left() << ") is less than the computed number "
      "of required input items (" << n_bits_to_input <<
      ") ... using lower number.\n";
    n_bits_to_input = d_in_buf->n_items_left();
    n_bits_to_output = compute_n_output_bits (n_bits_to_input);
  }

  // set the correct number of I/O bits

  d_n_bits_to_input = n_bits_to_input;
  d_n_bits_to_output = n_bits_to_output;

  if (DO_PRINT_DEBUG) {
    std::cout <<
      "Before Encoding{output}:\n"
      "  # output bits      = " << d_n_bits_to_output << "\n"
      "  # input bits       = " << d_n_bits_to_input << "\n"
      "  # output bits used = " << d_out_buf->n_items_used() << "\n"
      "  # input bits used  = " << d_in_buf->n_items_used() << "\n";
  }

  // call the private encode function

  encode_private ();

  if (DO_PRINT_DEBUG) {
    std::cout <<
      "After Encoding{output}:\n"
      "  # output bits      = " << d_n_bits_to_output << "\n"
      "  # input bits       = " << d_n_bits_to_input << "\n"
      "  # output bits used = " << d_out_buf->n_items_used() << "\n"
      "  # input bits used  = " << d_in_buf->n_items_used() << "\n";
  }

  // return the actual number of input bits used

  return (n_bits_to_input - d_n_bits_to_input);
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
(const code_input_ptr in_buf,
 size_t n_bits_to_input,
 code_output_ptr out_buf)
{
  if (in_buf == 0) {
    std::cerr << "encoder::encode{input}: Error: input buffer is NULL.\n";
    assert (0);
  }
  if (out_buf == 0) {
    std::cerr << "encoder::encode{input}: Error: output buffer is NULL.\n";
    assert (0);
  }
  if (n_bits_to_input == 0) {
    std::cerr << "encoder::encode{input}: Warning: no input bits requested.\n";
    return (0);
  }

  // set the class-internal number of input and
  // output bits left to encode

  d_in_buf = in_buf;
  d_out_buf = out_buf;

  // check that there are enough input buffer items

  if (d_in_buf->n_items_left() < n_bits_to_input) {
    std::cerr << "encoder::encode{input}: Warning: input buffer size (" <<
      d_in_buf->n_items_left() << ") is less than the desired number "
      "of input items (" << n_bits_to_input <<
      ") ... using lower number.\n";
    n_bits_to_input = d_in_buf->n_items_left();
  }

  // check that there are enough output buffer items

  size_t n_bits_to_output = compute_n_output_bits (n_bits_to_input);

  if (d_out_buf->n_items_left() < n_bits_to_output) {
    std::cerr << "encoder::encode{input}: Warning: output buffer size (" <<
      d_out_buf->n_items_left() << ") is less than the computed number "
      "of required output items (" << n_bits_to_output <<
      ") ... using lower number.\n";
    n_bits_to_output = d_out_buf->n_items_left();
    n_bits_to_input = compute_n_input_bits (n_bits_to_output);
  }

  // set the correct number of I/O bits

  d_n_bits_to_input = n_bits_to_input;
  d_n_bits_to_output = n_bits_to_output;

  if (DO_PRINT_DEBUG) {
    std::cout <<
      "Before Encoding{input}:\n"
      "  # output bits      = " << d_n_bits_to_output << "\n"
      "  # input bits       = " << d_n_bits_to_input << "\n"
      "  # output bits used = " << d_out_buf->n_items_used() << "\n"
      "  # input bits used  = " << d_in_buf->n_items_used() << "\n";
  }

  // call the private encode function

  encode_private ();

  if (DO_PRINT_DEBUG) {
    std::cout <<
      "After Encoding{input}:\n"
      "  # output bits      = " << d_n_bits_to_output << "\n"
      "  # input bits       = " << d_n_bits_to_input << "\n"
      "  # output bits used = " << d_out_buf->n_items_used() << "\n"
      "  # input bits used  = " << d_in_buf->n_items_used() << "\n";
  }

  // return the actual number of output bits written

  return (n_bits_to_output - d_n_bits_to_output);
}
