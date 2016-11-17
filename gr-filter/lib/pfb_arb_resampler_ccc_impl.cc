/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include "pfb_arb_resampler_ccc_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>

namespace gr {
  namespace filter {

    pfb_arb_resampler_ccc::sptr
    pfb_arb_resampler_ccc::make(float rate,
                                const std::vector<gr_complex> &taps,
                                unsigned int filter_size)
    {
      return gnuradio::get_initial_sptr
        (new pfb_arb_resampler_ccc_impl(rate, taps, filter_size));
    }


    pfb_arb_resampler_ccc_impl::pfb_arb_resampler_ccc_impl(float rate,
                                                           const std::vector<gr_complex> &taps,
                                                           unsigned int filter_size)
      : block("pfb_arb_resampler_ccc",
              io_signature::make(1, 1, sizeof(gr_complex)),
              io_signature::make(1, 1, sizeof(gr_complex)))
    {
      d_updated = false;

      d_resamp = new kernel::pfb_arb_resampler_ccc(rate, taps, filter_size);
      set_history(d_resamp->taps_per_filter());
      set_relative_rate(rate);
    }

    pfb_arb_resampler_ccc_impl::~pfb_arb_resampler_ccc_impl()
    {
      delete d_resamp;
    }

    void
    pfb_arb_resampler_ccc_impl::forecast(int noutput_items,
                                         gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      if(noutput_items / relative_rate() < 1) {
        for(unsigned i = 0; i < ninputs; i++)
          ninput_items_required[i] = relative_rate() + history() - 1;
      }
      else {
        for(unsigned i = 0; i < ninputs; i++)
          ninput_items_required[i] = noutput_items/relative_rate() + history() - 1;
      }
    }

    void
    pfb_arb_resampler_ccc_impl::set_taps(const std::vector<gr_complex> &taps)
    {
      gr::thread::scoped_lock guard(d_mutex);

      d_resamp->set_taps(taps);
      set_history(d_resamp->taps_per_filter());
      d_updated = true;
    }

    std::vector<std::vector<gr_complex> >
    pfb_arb_resampler_ccc_impl::taps() const
    {
      return d_resamp->taps();
    }

    void
    pfb_arb_resampler_ccc_impl::print_taps()
    {
      d_resamp->print_taps();
    }

    void
    pfb_arb_resampler_ccc_impl::set_rate(float rate)
    {
      gr::thread::scoped_lock guard(d_mutex);

      d_resamp->set_rate(rate);
      set_relative_rate(rate);
    }

    void
    pfb_arb_resampler_ccc_impl::set_phase(float ph)
    {
      gr::thread::scoped_lock guard(d_mutex);
      d_resamp->set_phase(ph);
    }

    float
    pfb_arb_resampler_ccc_impl::phase() const
    {
      return d_resamp->phase();
    }

    unsigned int
    pfb_arb_resampler_ccc_impl::interpolation_rate() const
    {
      return d_resamp->interpolation_rate();
    }

    unsigned int
    pfb_arb_resampler_ccc_impl::decimation_rate() const
    {
      return d_resamp->decimation_rate();
    }

    float
    pfb_arb_resampler_ccc_impl::fractional_rate() const
    {
      return d_resamp->fractional_rate();
    }

    unsigned int
    pfb_arb_resampler_ccc_impl::taps_per_filter() const
    {
      return d_resamp->taps_per_filter();
    }

    int
    pfb_arb_resampler_ccc_impl::group_delay() const
    {
      return d_resamp->group_delay();
    }

    float
    pfb_arb_resampler_ccc_impl::phase_offset(float freq, float fs)
    {
      return d_resamp->phase_offset(freq, fs);
    }

    int
    pfb_arb_resampler_ccc_impl::general_work(int noutput_items,
                                             gr_vector_int &ninput_items,
                                             gr_vector_const_void_star &input_items,
                                             gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_mutex);

      gr_complex *in = (gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      if(d_updated) {
        d_updated = false;
        return 0;              // history requirements may have changed.
      }

      int nitems_read;
      int nitems = floorf((float)noutput_items / relative_rate());
      int processed = d_resamp->filter(out, in, nitems, nitems_read);

      consume_each(nitems_read);
      return processed;
    }

  } /* namespace filter */
} /* namespace gr */
