/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2013 Free Software Foundation, Inc.
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

#include "stretch_ff_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    stretch_ff::sptr
    stretch_ff::make(float lo, size_t vlen)
    {
      return gnuradio::get_initial_sptr
        (new stretch_ff_impl(lo, vlen));
    }

    stretch_ff_impl::stretch_ff_impl(float lo, size_t vlen)
      : sync_block("stretch_ff",
                      io_signature::make(1, 1, vlen * sizeof(float)),
                      io_signature::make(1, 1, vlen * sizeof(float))),
        d_lo(lo), d_vlen(vlen)
    {
    }

    stretch_ff_impl::~stretch_ff_impl()
    {
    }

    int
    stretch_ff_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      const float *in = (const float *)input_items[0];
      float *out = (float *)output_items[0];

      for(int count = 0; count < noutput_items; count++) {
        float vmax = in[0] - d_lo;

        for(unsigned int i = 1; i < d_vlen; i++) {
          float vtmp = in[i] - d_lo;
          if(vtmp > vmax)
            vmax = vtmp;
        }

        if(vmax != 0.0)
          for(unsigned int i = 0; i < d_vlen; i++)
            out[i] = d_lo * (1.0 - (in[i] - d_lo) / vmax);
        else
          for(unsigned int i = 0; i < d_vlen; i++)
            out[i] = in[i];

        in  += d_vlen;
        out += d_vlen;
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
