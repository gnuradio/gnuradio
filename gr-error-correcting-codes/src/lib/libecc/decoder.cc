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

#include <decoder.h>
#include <assert.h>
#include <iostream>

#define DO_PRINT_DEBUG 1

#if DO_PRINT_DEBUG
#include <mld/n2bs.h>
#endif

/*
 * decode a certain number of output bits
 *
 * the 'in_buf' and 'out_buf' must have enough memory to handle the
 *     number of input metrics and output bits; no error checking is done!
 *
 * n_bits_to_output: the number of bits per output stream to output.
 *
 * returns the actual number of metrics used per input stream.
 */

size_t
decoder::decode
(const char** in_buf,
 char** out_buf,
 size_t n_bits_to_output)
{
  // set the class-internal number of input metrics
  // and output bits left to decode

  size_t saved_n_input_metrics;
  saved_n_input_metrics = d_n_input_metrics_left =
    compute_n_input_metrics (n_bits_to_output);
  d_n_output_bits_left = n_bits_to_output;

  // call the private decode function

  decode_private (in_buf, out_buf);

  if (DO_PRINT_DEBUG) {
    std::cout << "n_input_metrics_used = " <<
      (saved_n_input_metrics - d_n_input_metrics_left) << "\n"
      "n_output_bits_used = " <<
      (n_bits_to_output - d_n_output_bits_left) << '\n';
  }

  // return the actual number of input metrics used

  return (saved_n_input_metrics - d_n_input_metrics_left);
}

/*
 * decode a certain number of input metrics
 *
 * the 'in_buf' and 'out_buf' must have enough memory to handle the
 *     number of input metrics and output bits; no error checking is done!
 *
 * n_metrics_to_input: the number of metrics per input stream to decode
 *
 * returns the actual number of bits written per output stream
 */

size_t
decoder::decode
(const char** in_buf,	
 size_t n_metrics_to_input,
 char** out_buf)
{
  // set the class-internal number of input metrics and
  // output bits left to decode

  size_t saved_n_output_bits;
  saved_n_output_bits = d_n_output_bits_left =
    compute_n_output_bits (n_metrics_to_input);
  d_n_input_metrics_left = n_metrics_to_input;

  // call the private decode function

  decode_private (in_buf, out_buf);

  if (DO_PRINT_DEBUG) {
    std::cout << "n_input_metrics_used = " <<
      (n_metrics_to_input - d_n_input_metrics_left) << '\n';
    std::cout << "n_output_bits_used = " <<
      (saved_n_output_bits - d_n_output_bits_left) << '\n';
  }

  // return the actual number of output bits written

  return (saved_n_output_bits - d_n_output_bits_left);
}
