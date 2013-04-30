/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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

#include <gnuradio/atsc/basic_trellis_encoder_impl.h>
#include <assert.h>

const unsigned char atsci_basic_trellis_encoder::next_state[32] = {
  0,1,4,5,
  2,3,6,7,
  1,0,5,4,
  3,2,7,6,
  4,5,0,1,
  6,7,2,3,
  5,4,1,0,
  7,6,3,2
};

const unsigned char atsci_basic_trellis_encoder::out_symbol[32] = {
  0,2,4,6,
  1,3,5,7,
  0,2,4,6,
  1,3,5,7,
  4,6,0,2,
  5,7,1,3,
  4,6,0,2,
  5,7,1,3
};


/*!
 * Encode two bit INPUT into 3 bit return value.  Domain is [0,3],
 * Range is [0,7].  The mapping to bipolar levels is not done.
 */

int
atsci_basic_trellis_encoder::encode (unsigned int input)
{
  assert (input < 4);
  int index = (state << 2) + input;
  state = next_state[index];
  return out_symbol[index];
}

