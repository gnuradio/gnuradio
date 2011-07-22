/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010,2011 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <digital_correlate_access_code_bb.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <gr_count_bits.h>
#include <cstdio>


#define VERBOSE 0


digital_correlate_access_code_bb_sptr
digital_make_correlate_access_code_bb (const std::string &access_code, int threshold)
{
  return gnuradio::get_initial_sptr(new digital_correlate_access_code_bb
				    (access_code, threshold));
}


digital_correlate_access_code_bb::digital_correlate_access_code_bb (
  const std::string &access_code, int threshold)
  : gr_sync_block ("correlate_access_code_bb",
		   gr_make_io_signature (1, 1, sizeof(char)),
		   gr_make_io_signature (1, 1, sizeof(char))),
    d_data_reg(0), d_flag_reg(0), d_flag_bit(0), d_mask(0),
    d_threshold(threshold)

{
  if (!set_access_code(access_code)){
    fprintf(stderr, "digital_correlate_access_code_bb: access_code is > 64 bits\n");
    throw std::out_of_range ("access_code is > 64 bits");
  }
}

digital_correlate_access_code_bb::~digital_correlate_access_code_bb ()
{
}

bool
digital_correlate_access_code_bb::set_access_code(
  const std::string &access_code)
{
  unsigned len = access_code.length();	// # of bytes in string
  if (len > 64)
    return false;

  // set len top bits to 1.
  d_mask = ((~0ULL) >> (64 - len)) << (64 - len);

  d_flag_bit = 1LL << (64 - len);	// Where we or-in new flag values.
                                        // new data always goes in 0x0000000000000001
  d_access_code = 0;
  for (unsigned i=0; i < 64; i++){
    d_access_code <<= 1;
    if (i < len)
      d_access_code |= access_code[i] & 1;	// look at LSB only
  }

  return true;
}

int
digital_correlate_access_code_bb::work (int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];

  for (int i = 0; i < noutput_items; i++){

    // compute output value
    unsigned int t = 0;

    t |= ((d_data_reg >> 63) & 0x1) << 0;
    t |= ((d_flag_reg >> 63) & 0x1) << 1;	// flag bit
    out[i] = t;
    
    // compute hamming distance between desired access code and current data
    unsigned long long wrong_bits = 0;
    unsigned int nwrong = d_threshold+1;
    int new_flag = 0;

    wrong_bits  = (d_data_reg ^ d_access_code) & d_mask;
    nwrong = gr_count_bits64(wrong_bits);

    // test for access code with up to threshold errors
    new_flag = (nwrong <= d_threshold);

#if VERBOSE
    if(new_flag) {
      fprintf(stderr, "access code found: %llx\n", d_access_code);
    }
    else {
      fprintf(stderr, "%llx  ==>  %llx\n", d_access_code, d_data_reg);
    }
#endif

    // shift in new data and new flag
    d_data_reg = (d_data_reg << 1) | (in[i] & 0x1);
    d_flag_reg = (d_flag_reg << 1);
    if (new_flag) {
      d_flag_reg |= d_flag_bit;
    }
  }

  return noutput_items;
}
  
