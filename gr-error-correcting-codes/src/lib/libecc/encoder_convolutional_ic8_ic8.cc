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

#include <encoder_convolutional_ic8_ic8.h>
#include <assert.h>
#include <iostream>

#define DO_TIME_THOUGHPUT 1
#define DO_PRINT_DEBUG 1

#if DO_TIME_THOUGHPUT
#include <mld/mld_timer.h>
#endif
#if DO_PRINT_DEBUG
#include <mld/n2bs.h>
#endif

// FIXME: when doing packed, should probably allow user to select how
// bits are selected, so-as to make sure it's always the same
// no matter the CPU endianness

// FIXME
size_t
encoder_convolutional_ic8_ic8::compute_n_output_bits
(size_t n_input_bits)
{
  assert (0);
  return (0);
}

/*
 * Compute the number of input bits needed to produce
 * 'n_output' bits.  For convolutional encoders, there is
 * 1 bit output per bit input per stream, with the addition of a some
 * bits for trellis termination if selected.  Thus the input:output
 * bit ratio will be:
 * 
 * if (streaming | no termination), 1:1
 *
 * if (not streaming & termination), roughly 1:(1+X), where "X" is the
 * total memory size of the code divided by the block length in bits.
 * But this also depends on the state of the FSM ... how many bits are
 * left before termination.
 *
 * The returned value will also depend on whether bits are packed, as
 * well as whether streams are mux'ed together.
 */

size_t
encoder_convolutional_ic8_ic8::compute_n_input_bits
(size_t n_output_bits)
{
  size_t t_n_output_bits, t_n_input_bits;
  t_n_output_bits = t_n_input_bits = n_output_bits;

  if (d_do_termination == true) {

    // not streaming, doing termination; find the number of bits
    // currently available with no required inputs, if any

    size_t n_extra = 0;
    if (d_fsm_state == fsm_enc_conv_doing_term) {
      n_extra = d_max_memory - d_n_enc_bits;
    }

    // check to see if this is enough; return 0 if it is.

    if (n_extra >= t_n_output_bits)
      return (0);

    // remove those which require no input

    t_n_output_bits -= n_extra;

    // find the number of frames of data which could be processed

    size_t t_n_output_bits_per_frame = d_frame_size_bits + d_max_memory;

    // get the base number of input items required for the given
    // number of frames to be generated

    size_t t_n_frames = t_n_output_bits / t_n_output_bits_per_frame;
    t_n_input_bits = t_n_frames * d_frame_size_bits;

    // add to that the number of leftover inputs needed to generate
    // the remainder of the outputs within the remaining frame, up to
    // the given frame size (since anything beyond that within this
    // frame requires no inputs)

    size_t t_leftover_bits = t_n_output_bits % t_n_output_bits_per_frame;
    t_n_input_bits += ((t_leftover_bits > d_frame_size_bits) ?
		       d_frame_size_bits : t_leftover_bits);
  }

  return (t_n_input_bits);
}

void
encoder_convolutional_ic8_ic8::increment_io_indices
(bool while_encoding)
{
  // increment the buffer index only for this version, only after
  // encoding is done and all resulting outputs are stored on the
  // output streams

  if (while_encoding == false) {
    d_out_buf_ndx++;
    d_in_buf_ndx++;
  }

  // nothing to do while encoding, so no else

#if 0
// move counters to the next input bit, wrapping to the next input
// byte as necessary
  if (++d_in_bit_shift % g_num_bits_per_byte == 0) {
    d_in_bit_shift = 0;
    d_in_buf_ndx++;
  }
// move counters to the next output bit, wrapping to the next output
// byte as necessary
    if (++d_out_bit_shift % g_num_bits_per_byte == 0) {
      d_out_bit_shift = 0;
      d_out_buf_ndx++;
    }
#endif
}

void
encoder_convolutional_ic8_ic8::output_bit
(char t_out_bit,
 char** out_buf,
 size_t t_output_stream)
{
  // store the result for this particular output stream
  // one bit per output item for "ic8" type output

  if (DO_PRINT_DEBUG) {
    std::cout << ", O_i[" << t_output_stream <<
      "][" << d_out_buf_ndx << "] = " <<
      n2bs (out_buf[t_output_stream][d_out_buf_ndx], 2);
  }

  out_buf[t_output_stream][d_out_buf_ndx] = t_out_bit;

  if (DO_PRINT_DEBUG) {
    std::cout << ", b_out = " << t_out_bit <<
      ", O_o[" << t_output_stream << "][" << d_out_buf_ndx << "][" <<
      d_out_bit_shift << "] = " <<
      n2bs (out_buf[t_output_stream][d_out_buf_ndx], 2) << '\n';
  }

#if 0
#if DO_PRINT_DEBUG
  std::cout << ", O_i[" << t_output_stream <<
    "][" << d_out_buf_ndx << "] = " <<
    n2bs (out_buf[t_output_stream][d_out_buf_ndx], g_num_bits_per_byte);
#endif

// packed bits in each output item
  out_buf[t_output_stream][d_out_buf_ndx] |=
    (t_out_bit << d_out_bit_shift);

#if DO_PRINT_DEBUG
  std::cout << ", b_out = " << t_out_bit <<
    ", O_o[" << t_output_stream << "][" << d_out_buf_ndx << "][" <<
    d_out_bit_shift << "] = " <<
    n2bs (out_buf[t_output_stream][d_out_buf_ndx], g_num_bits_per_byte) << '\n';
#endif
#endif
}

char
encoder_convolutional_ic8_ic8::get_next_bit__input
(const char** in_buf,
 size_t code_input_n)
{
  // get a bit from this particular input stream
  // one bit per output item for "ic8" type input

  if (DO_PRINT_DEBUG) {
    std::cout << "I[" << p << "][" << d_in_buf_ndx << "] = ";
    cout_binary (t_next_bit, g_num_bits_per_byte);
    std::cout << ", st_i[" << p << "] = ";
    cout_binary ((*t_states_ptr), d_max_memory+2);
    std::cout << ", I[" << p << "][" << d_in_buf_ndx << "][" <<
      d_in_bit_shift << "] = " << t_next_bit <<
      ", st_o[" << p << "] = ";
    cout_binary (t_state, d_max_memory+2);
    std::cout << '\n';
  }

   return ((in_buf[code_input_n][d_in_buf_ndx] >> d_in_bit_shift) & 1);
}

char
encoder_convolutional_ic8_ic8::get_next_bit__term
(size_t code_input_n)
{
  return ((d_term_states[code_input_n] >> d_in_bit_shift) & 1);
}
