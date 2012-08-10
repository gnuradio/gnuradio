/* -*- c++ -*- */
/*
 * Copyright 2006,2010-2012 Free Software Foundation, Inc.
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

#include "binary_slicer_fb_impl.h"
#include <gr_io_signature.h>
#include <gr_math.h>

namespace gr {
  namespace digital {
    
    binary_slicer_fb::sptr binary_slicer_fb::make()
    {
      return gnuradio::get_initial_sptr(new binary_slicer_fb_impl());
    }

    binary_slicer_fb_impl::binary_slicer_fb_impl()
      : gr_sync_block("binary_slicer_fb",
		      gr_make_io_signature(1, 1, sizeof(float)),
		      gr_make_io_signature(1, 1, sizeof(unsigned char)))
    {
    }

    binary_slicer_fb_impl::~binary_slicer_fb_impl()
    {
    }

    int
    binary_slicer_fb_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const float *in = (const float *)input_items[0];
      unsigned char *out = (unsigned char *)output_items[0];

      for(int i = 0; i < noutput_items; i++) {
	out[i] = gr_binary_slicer(in[i]);
      }
  
      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
