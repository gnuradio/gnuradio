/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#include "adaptive_fir_ccf_impl.h"
#include <gr_io_signature.h>

namespace gr {
  namespace filter {

    adaptive_fir_ccf::sptr adaptive_fir_ccf::make(const char *name, int decimation,
						  const std::vector<float> &taps)
    {
      return gnuradio::get_initial_sptr(new adaptive_fir_ccf_impl
					(name, decimation, taps));
    }

    adaptive_fir_ccf_impl::adaptive_fir_ccf_impl(const char *name, int decimation,
						 const std::vector<float> &taps)
      : gr_sync_decimator(name,
			  gr_make_io_signature(1, 1, sizeof(gr_complex)),
			  gr_make_io_signature(1, 1, sizeof(gr_complex)),
			  decimation),
	kernel::adaptive_fir_ccf(decimation, taps),
	d_updated(false)
    {
      set_history(d_ntaps);
    }

    void
    adaptive_fir_ccf_impl::set_taps(const std::vector<float> &taps)
    {
      d_new_taps = taps;
      d_updated = true;
    }

    std::vector<float>
    adaptive_fir_ccf_impl::taps()
    {
      return kernel::fir_filter_ccf::taps();
    }   
    
    float
    adaptive_fir_ccf_impl::error(const gr_complex &out)
    {
      return 0;
    }

    void
    adaptive_fir_ccf_impl::update_tap(float &tap, const gr_complex &in)
    {
      tap = tap;
    }

    int
    adaptive_fir_ccf_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      gr_complex *in = (gr_complex *)input_items[0];
      gr_complex *out = (gr_complex *)output_items[0];

      if (d_updated) {
	kernel::fir_filter_ccf::set_taps(d_new_taps);
	set_history(d_ntaps);
	d_updated = false;
	return 0;		     // history requirements may have changed.
      }

      // Call base class filtering function that uses
      // overloaded error and update_tap functions.
      if (decimation() == 1) {
	filterN(out, in, noutput_items);
      }
      else {
	filterNdec(out, in, noutput_items,
		   decimation());
      }

      return noutput_items;
    }

  } /* namespace filter */
} /* namespace gr */
