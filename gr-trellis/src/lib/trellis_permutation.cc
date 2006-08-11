/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#include <trellis_permutation.h>
#include <gr_io_signature.h>
#include <iostream>

trellis_permutation_sptr 
trellis_make_permutation (const int K, const std::vector<int> &TABLE, const size_t NBYTES)
{
  return trellis_permutation_sptr (new trellis_permutation (K,TABLE,NBYTES));
}

trellis_permutation::trellis_permutation (const int K, const std::vector<int> &TABLE, const size_t NBYTES)
  : gr_sync_block ("permutation",
		   gr_make_io_signature (1, -1, NBYTES),
		   gr_make_io_signature (1, -1, NBYTES)),
    d_K (K),
    d_TABLE (TABLE),
    d_NBYTES (NBYTES)
{
    set_output_multiple (d_K);
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
  //std::cout << noutput_items << "\n";

  for (int m=0;m<nstreams;m++) {
    const char *in = (const char *) input_items[m];
    char *out = (char *) output_items[m];

    // per stream processing
    for (unsigned int i = 0; i < noutput_items; i++){
      //std::cout << i << " " << i*d_NBYTES << " " << (d_K*(i/d_K)+d_TABLE[i%d_K])*d_NBYTES  << "\n";
      memcpy(&(out[i*d_NBYTES]), &(in[(d_K*(i/d_K)+d_TABLE[i%d_K])*d_NBYTES]), d_NBYTES);
    }
    // end per stream processing
  }
  return noutput_items;
}
