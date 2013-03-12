/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2013 Free Software Foundation, Inc.
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

#include "rms_cf_impl.h"
#include <gr_io_signature.h>
#include <cmath>

namespace gr {
  namespace blocks {

    rms_cf::sptr
    rms_cf::make(double alpha)
    {
      return gnuradio::get_initial_sptr
        (new rms_cf_impl(alpha));
    }

    rms_cf_impl::rms_cf_impl (double alpha)
      : gr_sync_block("rms_cf",
                      gr_make_io_signature(1, 1, sizeof(gr_complex)),
                      gr_make_io_signature(1, 1, sizeof(float))),
        d_iir(alpha)
    {
    }

    rms_cf_impl::~rms_cf_impl()
    {
    }

    void
    rms_cf_impl::set_alpha(double alpha)
    {
      d_iir.set_taps(alpha);
    }

    int
    rms_cf_impl::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *)input_items[0];
      float *out = (float *)output_items[0];

      for(int i = 0; i < noutput_items; i++) {
        double mag_sqrd = in[i].real()*in[i].real() + in[i].imag()*in[i].imag();
        double f = d_iir.filter(mag_sqrd);
        out[i] = sqrt(f);
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
