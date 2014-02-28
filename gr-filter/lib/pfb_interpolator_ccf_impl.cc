/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012 Free Software Foundation, Inc.
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

#include "pfb_interpolator_ccf_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace filter {

    pfb_interpolator_ccf::sptr
    pfb_interpolator_ccf::make(unsigned int interp,
			       const std::vector<float> &taps)
    {
      return gnuradio::get_initial_sptr
	(new pfb_interpolator_ccf_impl(interp, taps));
    }


    pfb_interpolator_ccf_impl::pfb_interpolator_ccf_impl(unsigned int interp,
							 const std::vector<float> &taps)
      : sync_interpolator("pfb_interpolator_ccf",
			     io_signature::make(1, 1, sizeof(gr_complex)),
			     io_signature::make(1, 1, sizeof(gr_complex)),
			     interp),
	polyphase_filterbank(interp, taps),
	d_updated (false), d_rate(interp)
    {
      set_history(d_taps_per_filter);
    }

    pfb_interpolator_ccf_impl::~pfb_interpolator_ccf_impl()
    {
    }

    void
    pfb_interpolator_ccf_impl::set_taps(const std::vector<float> &taps)
    {
      gr::thread::scoped_lock guard(d_mutex);

      polyphase_filterbank::set_taps(taps);
      set_history(d_taps_per_filter);
      d_updated = true;
    }

    void
    pfb_interpolator_ccf_impl::print_taps()
    {
      polyphase_filterbank::print_taps();
    }

    std::vector<std::vector<float> >
    pfb_interpolator_ccf_impl::taps() const
    {
      return polyphase_filterbank::taps();
    }

    int
    pfb_interpolator_ccf_impl::work(int noutput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
    {
      gr_complex *in = (gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      if(d_updated) {
	d_updated = false;
	return 0;		     // history requirements may have changed.
      }

      int i = 0, count = 0;

      while(i < noutput_items) {
	for(unsigned int j = 0; j < d_rate; j++) {
	  out[i] = d_fir_filters[j]->filter(&in[count]);
	  i++;
	}
	count++;
      }

      return i;
    }

  } /* namespace filter */
} /* namespace gr */
