/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#include "permutation_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>
#include <string.h>

namespace gr {
  namespace trellis {

    permutation::sptr
    permutation::make(int K, const std::vector<int> &TABLE,
		      int SYMS_PER_BLOCK, size_t BYTES_PER_SYMBOL)
    {
      return gnuradio::get_initial_sptr
	(new permutation_impl(K,TABLE,SYMS_PER_BLOCK,BYTES_PER_SYMBOL));
    }

    permutation_impl::permutation_impl(int K, const std::vector<int> &TABLE,
				       int SYMS_PER_BLOCK, size_t BYTES_PER_SYMBOL)
      : sync_block("permutation",
		      io_signature::make(1, -1, BYTES_PER_SYMBOL),
		      io_signature::make(1, -1, BYTES_PER_SYMBOL)),
	d_K(K), d_TABLE(TABLE),
	d_SYMS_PER_BLOCK(SYMS_PER_BLOCK),
	d_BYTES_PER_SYMBOL(BYTES_PER_SYMBOL)
    {
      set_output_multiple(d_K*d_SYMS_PER_BLOCK);
      //std::cout << d_K << "\n";
    }

    void permutation_impl::set_K(int K)
    {
      gr::thread::scoped_lock guard(d_setlock);
      d_K=K;
      set_output_multiple(d_K*d_SYMS_PER_BLOCK);
    }

    void permutation_impl::set_TABLE (const std::vector<int> &table) 
    {
      gr::thread::scoped_lock guard(d_setlock);
      d_TABLE = table; 
    }

    void permutation_impl::set_SYMS_PER_BLOCK(int spb)
    {
      gr::thread::scoped_lock guard(d_setlock);
      d_SYMS_PER_BLOCK=spb;
      set_output_multiple(d_K*d_SYMS_PER_BLOCK);
    }

    permutation_impl::~permutation_impl()
    {
    }

    int
    permutation_impl::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_setlock);
      int nstreams = input_items.size();

      for(int m=0;m<nstreams;m++) {
	const char *in = (const char *) input_items[m];
	char *out = (char *) output_items[m];

	// per stream processing
	for(int i = 0; i < noutput_items/d_SYMS_PER_BLOCK; i++) {
	  // Index i refers to blocks.
	  // Beginning of packet (in blocks)
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

  } /* namespace trellis */
} /* namespace gr */
