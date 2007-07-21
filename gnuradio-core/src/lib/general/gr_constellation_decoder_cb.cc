/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#include <gr_constellation_decoder_cb.h>
#include <gr_io_signature.h>
#include <stdexcept>

#include <iostream>
using std::cout;
using std::endl;

static const bool compute_EVM = false;

gr_constellation_decoder_cb_sptr
gr_make_constellation_decoder_cb (const std::vector<gr_complex> &sym_position, 
				  const std::vector<unsigned char> &sym_value_out)
{
  return gr_constellation_decoder_cb_sptr 
		(new gr_constellation_decoder_cb(sym_position, sym_value_out));
}

gr_constellation_decoder_cb::
gr_constellation_decoder_cb (const std::vector<gr_complex> &sym_position, 
			     const std::vector<unsigned char> &sym_value_out)
  : gr_sync_block ("constellation_decoder_cb",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (unsigned char)))	
{
  if (!set_constellation(sym_position,sym_value_out))
    throw std::invalid_argument("constellation_decoder_cb");
}


gr_constellation_decoder_cb::~gr_constellation_decoder_cb(){}


bool
gr_constellation_decoder_cb::set_constellation(const std::vector<gr_complex> &sym_position, 
			                       const std::vector<unsigned char> &sym_value_out)
{
  if (sym_position.size() != sym_value_out.size())
    return false;

  if (sym_position.size()<1)
    return false;

  d_sym_position  = sym_position;
  d_sym_value_out = sym_value_out;	
  return true;
}


int
gr_constellation_decoder_cb::work(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  gr_complex const *in = (const gr_complex *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];
  unsigned int table_size = d_sym_value_out.size();
  unsigned int min_index = 0;
  float min_euclid_dist = 0;
  float euclid_dist = 0;
  double total_error = 0;
    
  for(int i = 0; i < noutput_items; i++){
    min_euclid_dist = norm(in[i] - d_sym_position[0]); 
    min_index = 0; 
    for (unsigned int j = 1; j < table_size; j++){
      euclid_dist = norm(in[i] - d_sym_position[j]);
      if (euclid_dist < min_euclid_dist){
	min_euclid_dist = euclid_dist;
	min_index = j;
      }
    }

    out[i] = d_sym_value_out[min_index];

    if (compute_EVM)
      total_error += sqrtf(min_euclid_dist);
  }

  if (compute_EVM){
    double mean = total_error / noutput_items;
    double rms = sqrt(mean * mean);
    fprintf(stderr, "EVM = %8.4f\n", rms);
  }

  return noutput_items;
}
