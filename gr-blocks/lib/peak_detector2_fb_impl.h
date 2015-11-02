/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PEAK_DETECTOR2_FB_IMPL_H
#define INCLUDED_GR_PEAK_DETECTOR2_FB_IMPL_H

#include <gnuradio/blocks/peak_detector2_fb.h>

namespace gr {
  namespace blocks {

    class peak_detector2_fb_impl : public peak_detector2_fb
    {
    private:
      float d_threshold_factor_rise;
      int d_look_ahead;
      int d_look_ahead_remaining;
      int d_peak_ind;
      float d_peak_val;
      float d_alpha;
      float d_avg;
      bool d_found;

      void invalidate();

    public:
      peak_detector2_fb_impl(float threshold_factor_rise,
                             int look_ahead, float alpha);
      ~peak_detector2_fb_impl();

      void set_threshold_factor_rise(float thr);
      void set_look_ahead(int look);
      void set_alpha(float alpha);

      float threshold_factor_rise() { return d_threshold_factor_rise; }
      int look_ahead() { return d_look_ahead; }
      float alpha() { return d_alpha; }

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_PEAK_DETECTOR2_FB_IMPL_H */
