/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2013,2015 Free Software Foundation, Inc.
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

#include "peak_detector2_fb_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>
#include <limits>

namespace gr {
  namespace blocks {

    peak_detector2_fb::sptr
    peak_detector2_fb::make(float threshold_factor_rise,
                            int look_ahead, float alpha)
    {
      return gnuradio::get_initial_sptr
        (new peak_detector2_fb_impl(threshold_factor_rise,
                                    look_ahead, alpha));
    }

    peak_detector2_fb_impl::peak_detector2_fb_impl(float threshold_factor_rise,
                                                   int look_ahead, float alpha)
      : sync_block("peak_detector2_fb",
                   io_signature::make(1, 1, sizeof(float)),
                   io_signature::make2(1, 2, sizeof(char), sizeof(float))),
        d_avg(0.0f), d_found(false)
    {
      set_threshold_factor_rise(threshold_factor_rise);
      set_look_ahead(look_ahead);
      set_alpha(alpha);
    }

    peak_detector2_fb_impl::~peak_detector2_fb_impl()
    {
    }

    void
    peak_detector2_fb_impl::set_threshold_factor_rise(float thr)
    {
      gr::thread::scoped_lock lock(d_setlock);
      d_threshold_factor_rise = thr;
      invalidate();
    }

    void
    peak_detector2_fb_impl::set_look_ahead(int look)
    {
      gr::thread::scoped_lock lock(d_setlock);
      d_look_ahead = look;
      invalidate();
    }

    void
    peak_detector2_fb_impl::set_alpha(float alpha)
    {
      d_alpha = alpha;
    }

    void
    peak_detector2_fb_impl::invalidate()
    {
      d_found = false;
      set_output_multiple(1);
    }

    int
    peak_detector2_fb_impl::work(int noutput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items)
    {
      float *iptr = (float *)input_items[0];
      char *optr = (char *)output_items[0];
      float *sigout;

      if(output_items.size() == 2)
        sigout = (float *)output_items[1];

      memset(optr, 0, noutput_items*sizeof(char));

      gr::thread::scoped_lock lock(d_setlock);

      // have not crossed threshold yet
      if(d_found==false) {
        for(int i = 0; i < noutput_items; i++) {
          d_avg = d_alpha*iptr[i] + (1.0f - d_alpha)*d_avg;
          if(output_items.size() == 2)
            sigout[i]=d_avg;
          if(iptr[i] > d_avg * (1.0f + d_threshold_factor_rise)) {
            d_found = true;
            d_peak_val = std::numeric_limits<float>::min();
            set_output_multiple(d_look_ahead);
            return i;
          }
        }
        return noutput_items;
      } // end d_found==false

      // can complete in this call
      else if(noutput_items >= d_look_ahead) {
        for(int i = 0; i < d_look_ahead; i++) {
          d_avg = d_alpha*iptr[i] + (1.0f - d_alpha)*d_avg;
          if(output_items.size() == 2)
            sigout[i]=d_avg;
          if(iptr[i] > d_peak_val) {
            d_peak_val = iptr[i];
            d_peak_ind =i;
          }
        }
        optr[d_peak_ind] = 1;

        // restart the search
        invalidate();
        return d_look_ahead;
      } // end can complete in this call

      // cannot complete in this call
      else {
        return 0; // ask for more
      }
    }

  } /* namespace blocks */
} /* namespace gr */
