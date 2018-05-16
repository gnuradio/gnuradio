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

#include "filter_delay_fc_impl.h"
#include <volk/volk.h>

namespace gr {
  namespace filter {

    filter_delay_fc::sptr filter_delay_fc::make(const std::vector<float> &taps)
    {
      return gnuradio::get_initial_sptr(new filter_delay_fc_impl(taps));
    }

    filter_delay_fc_impl::filter_delay_fc_impl(const std::vector<float> &taps)
      : sync_block("filter_delay_fc",
		      io_signature::make(1, 2, sizeof(float)),
		      io_signature::make(1, 1, sizeof(gr_complex))),
	d_update(false)
    {
      d_taps = taps;
      d_fir = new kernel::fir_filter_fff(1, taps);
      d_delay = d_fir->ntaps() / 2;
      set_history(d_fir->ntaps());

      const int alignment_multiple =
	volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1, alignment_multiple));
    }

    filter_delay_fc_impl::~filter_delay_fc_impl()
    {
      delete d_fir;
    }

    std::vector<float>
    filter_delay_fc_impl::taps()
    {
      return d_fir->taps();
    }

    void
    filter_delay_fc_impl::set_taps(const std::vector<float> &taps)
    {
      // don't reset taps in case history changes
      d_taps = taps;
      d_update = true;
    }

    int
    filter_delay_fc_impl::work(int noutput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
    {
      float *in0 = (float *)input_items[0];
      float *in1;
      gr_complex *out = (gr_complex *)output_items[0];

      if(d_update) {
	d_fir->set_taps(d_taps);
	d_delay = d_fir->ntaps() / 2;
	set_history(d_fir->ntaps());
	return 0;
      }

      switch(input_items.size ()) {
      case 1:
	for(int i = 0; i < noutput_items; i++) {
	  out[i] = gr_complex(in0[i + d_delay],
			      d_fir->filter(&in0[i]));
	}
	break;

      case 2:
        in1 = (float *)input_items[1];
	for(int j = 0; j < noutput_items; j++) {
	  out[j] = gr_complex(in0[j + d_delay],
			      d_fir->filter(&in1[j]));
	}
	break;

      default:
	assert(0);
      }

      return noutput_items;
    }

  } /* namespace filter */
} /* namespace gr */
