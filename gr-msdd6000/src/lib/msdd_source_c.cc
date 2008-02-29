/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

//#define MSDD_DEBUG2_TRUE

#ifdef MSDD_DEBUG2_TRUE
#include <iostream>
#define MSDD_DEBUG2(x) std::cout << x << std::endl;
#else
#define MSDD_DEBUG2(x)
#endif

#include <msdd_source_c.h>
#include <gr_io_signature.h>

namespace {
  static const int NBASIC_SAMPLES_PER_ITEM = 2;	// I & Q
};

msdd_source_c_sptr
msdd_make_source_c (int which_board, 
		    int opp_mode,
		    const char *src, 
		    unsigned short port_src
		    ) throw (std::runtime_error)
{
  return msdd_source_c_sptr (new msdd_source_c (
  						which_board,
						opp_mode,
						src,
						port_src
						));
}

msdd_source_c::msdd_source_c (int which_board,
			      int opp_mode,
			      const char *src, 
			      unsigned short port_src
			      ) throw (std::runtime_error)
  : msdd_source_base ("msdd_source_c",
		      gr_make_io_signature (1, 1, 2 * sizeof (int)),
		      which_board, opp_mode, src, port_src
		      )
{

  switch (sizeof_basic_sample()) {	
  case 4:
	  d_buffer_copy_behavior.reset(
	   new msdd::BufferCopyBehaviorComplex <short> ());
    break;
  default:
    assert(false);    
  }				  
  
}

msdd_source_c::~msdd_source_c ()
{
}

int
msdd_source_c::ninput_bytes_reqd_for_noutput_items (int noutput_items)
{
  return noutput_items * NBASIC_SAMPLES_PER_ITEM * sizeof_basic_sample();
}

/*
 * Copy 8 bit fft from mdss buffer into output buffer
 */
void
msdd_source_c::copy_from_msdd_buffer (gr_vector_void_star &output_items,
				      int output_index,
				      int output_items_available,
				      int &output_items_produced,
				      const void *msdd_buffer,
				      int buffer_length,
				      int &bytes_read)
{
  unsigned nmsdd_bytes_per_item = NBASIC_SAMPLES_PER_ITEM * sizeof_basic_sample();
  
  unsigned int nitems = std::min (output_items_available,
				  (int)(buffer_length / nmsdd_bytes_per_item));
  
  (*d_buffer_copy_behavior.get())(output_items, msdd_buffer, output_index, nitems);
  
  output_items_produced = nitems;
  bytes_read = nitems * nmsdd_bytes_per_item;
}
