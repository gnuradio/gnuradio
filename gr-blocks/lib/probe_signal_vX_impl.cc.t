/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012-2013 Free Software Foundation, Inc.
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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <@NAME_IMPL@.h>
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    @NAME@::sptr
    @BASE_NAME@::make(size_t size)
    {
      return gnuradio::get_initial_sptr
        (new @NAME_IMPL@(size));
    }

    @NAME_IMPL@::@NAME_IMPL@(size_t size)
    : sync_block("@BASE_NAME@",
                    io_signature::make(1, 1, size*sizeof(@TYPE@)),
                    io_signature::make(0, 0, 0)),
      d_level(size, 0), d_size(size)
    {
    }

    @NAME_IMPL@::~@NAME_IMPL@()
    {
    }

    int
    @NAME_IMPL@::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      const @TYPE@ *in = (const @TYPE@ *)input_items[0];

      for(size_t i=0; i<d_size; i++)
	d_level[i] = in[(noutput_items-1)*d_size+i];

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
