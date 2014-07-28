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

#include "hilbert_fc_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace filter {

    hilbert_fc::sptr
    hilbert_fc::make(unsigned int ntaps,
                     firdes::win_type window,
                     double beta)
    {
      return gnuradio::get_initial_sptr
        (new hilbert_fc_impl(ntaps, window, beta));
    }

    hilbert_fc_impl::hilbert_fc_impl(unsigned int ntaps,
                                     firdes::win_type window,
                                     double beta)
      : sync_block("hilbert_fc",
                   io_signature::make(1, 1, sizeof(float)),
                   io_signature::make(1, 1, sizeof(gr_complex))),
	d_ntaps(ntaps | 0x1)	// ensure ntaps is odd
    {
      d_hilb = new kernel::fir_filter_fff(1,
                         firdes::hilbert(d_ntaps, window, beta));
      set_history(d_ntaps);

      const int alignment_multiple =
	volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1, alignment_multiple));
    }

    hilbert_fc_impl::~hilbert_fc_impl()
    {
      delete d_hilb;
    }

    int
    hilbert_fc_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      float *in = (float *)input_items[0];
      gr_complex *out = (gr_complex *)output_items[0];

      for(int i = 0; i < noutput_items; i++) {
	out[i] = gr_complex(in[i + d_ntaps/2],
			    d_hilb->filter(&in[i]));
      }

      return noutput_items;
    }

  } /* namespace filter */
} /* namespace gr */
