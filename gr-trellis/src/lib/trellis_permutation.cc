/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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

#include <trellis_permutation.h>
#include <gr_io_signature.h>
#include <iostream>
#include <string.h>

trellis_permutation_sptr 
trellis_make_permutation (int K, const std::vector<int> &TABLE, int SYMS_PER_BLOCK, size_t BYTES_PER_SYMBOL)
{
  return gnuradio::get_initial_sptr(new trellis_permutation (K,TABLE,SYMS_PER_BLOCK,BYTES_PER_SYMBOL));
}

trellis_permutation::trellis_permutation (int K, const std::vector<int> &TABLE, int SYMS_PER_BLOCK, size_t BYTES_PER_SYMBOL)
  : gr_sync_block ("permutation",
		   gr_make_io_signature (1, -1, BYTES_PER_SYMBOL),
		   gr_make_io_signature (1, -1, BYTES_PER_SYMBOL)),
    d_K (K),
    d_TABLE (TABLE),
    d_SYMS_PER_BLOCK (SYMS_PER_BLOCK),
    d_BYTES_PER_SYMBOL (BYTES_PER_SYMBOL)
{
    set_output_multiple (d_K*SYMS_PER_BLOCK);
    //std::cout << d_K << "\n";
}



int 
trellis_permutation::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  int nstreams = input_items.size();
  assert (input_items.size() == output_items.size());
  assert (noutput_items % d_K ==0);

  for (int m=0;m<nstreams;m++) {
    const char *in = (const char *) input_items[m];
    char *out = (char *) output_items[m];

    // per stream processing
    for (int i = 0; i < noutput_items/d_SYMS_PER_BLOCK; i++){
      // Index i refers to blocks.
      // Begining of packet (in blocks)
      int i0 = d_K*(i/d_K); 
      // position of block within packet (in blocks)
      int j0 = i%d_K;
      // new position of block within packet (in blocks)
      int k0 = d_TABLE[j0];
      memcpy(&(out[i*d_SYMS_PER_BLOCK*d_BYTES_PER_SYMBOL]), 
             &(in[(i0+k0)*d_SYMS_PER_BLOCK*d_BYTES_PER_SYMBOL]), 
             d_BYTES_PER_SYMBOL*d_SYMS_PER_BLOCK);
    }
    // end per stream processing
  }
  return noutput_items;
}
