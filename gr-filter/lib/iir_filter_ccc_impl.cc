/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2014 Free Software Foundation, Inc.
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

#include "iir_filter_ccc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace filter {

    iir_filter_ccc::sptr
    iir_filter_ccc::make(const std::vector<gr_complex> &fftaps,
			 const std::vector<gr_complex> &fbtaps,
			 bool oldstyle)
    {
      return gnuradio::get_initial_sptr
	(new iir_filter_ccc_impl(fftaps, fbtaps, oldstyle));
    }

    iir_filter_ccc_impl::iir_filter_ccc_impl(const std::vector<gr_complex> &fftaps,
					     const std::vector<gr_complex> &fbtaps,
					     bool oldstyle)

      : sync_block("iir_filter_ccc",
		   io_signature::make(1, 1, sizeof(gr_complex)),
		   io_signature::make(1, 1, sizeof(gr_complex))),
	d_updated(false)
    {
      d_iir = new kernel::iir_filter<gr_complex, gr_complex, gr_complex, gr_complex>(fftaps, fbtaps, oldstyle);
    }

    iir_filter_ccc_impl::~iir_filter_ccc_impl()
    {
      delete d_iir;
    }

    void
    iir_filter_ccc_impl::set_taps(const std::vector<gr_complex> &fftaps,
				  const std::vector<gr_complex> &fbtaps)
    {
      d_new_fftaps = fftaps;
      d_new_fbtaps = fbtaps;
      d_updated = true;
    }

    int
    iir_filter_ccc_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      if(d_updated) {
	d_iir->set_taps(d_new_fftaps, d_new_fbtaps);
	d_updated = false;
      }

      d_iir->filter_n(out, in, noutput_items);
      return noutput_items;
    };

  } /* namespace filter */
} /* namespace gr */

