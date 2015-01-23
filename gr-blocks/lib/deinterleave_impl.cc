/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include "deinterleave_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    deinterleave::sptr deinterleave::make(size_t itemsize, unsigned int blocksize)
    {
      return gnuradio::get_initial_sptr(new deinterleave_impl(itemsize, blocksize));
    }

    deinterleave_impl::deinterleave_impl(size_t itemsize, unsigned int blocksize)
      : block("deinterleave",
              io_signature::make (1, 1, itemsize),
              io_signature::make (1, io_signature::IO_INFINITE, itemsize)),
        d_itemsize(itemsize), d_blocksize(blocksize), d_current_output(0)
    {
      d_size_bytes = d_itemsize * d_blocksize;
      set_output_multiple(blocksize);
    }

    void
    deinterleave_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items * d_noutputs;
    }

    bool
    deinterleave_impl::check_topology(int ninputs, int noutputs)
    {
      set_relative_rate(1.0/(double)noutputs);
      d_noutputs = noutputs;
      return true;
    }

    int
    deinterleave_impl::general_work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star &input_items,
                                    gr_vector_void_star &output_items)
    {
      const char *in = (const char*)input_items[0];
      char **out = (char**)&output_items[0];

      int count = 0, totalcount = noutput_items*d_noutputs;
      unsigned int skip = 0;
      unsigned int acc = 0;
      while(count < totalcount) {
        memcpy(out[d_current_output]+skip*d_size_bytes, in, d_size_bytes);
        in += d_size_bytes;
        produce(d_current_output, d_blocksize);
        d_current_output = (d_current_output + 1) % d_noutputs;

        // accumulate times through the loop; increment skip after a
        // full pass over the output streams.
        // This is separate than d_current_output since we could be in
        // the middle of a loop when we exit.
        acc++;
        if(acc >= d_noutputs) {
          skip++;
          acc = 0;
        }

        // Keep track of our loop counter
        count+=d_blocksize;
      }
      consume_each(totalcount);
      return WORK_CALLED_PRODUCE;
    }


  } /* namespace blocks */
} /* namespace gr */
