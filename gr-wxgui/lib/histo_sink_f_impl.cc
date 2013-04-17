/* -*- c++ -*- */
/*
 * Copyright 2009-2011,2013 Free Software Foundation, Inc.
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

#include "histo_sink_f_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/math/special_functions/round.hpp>

namespace gr {
  namespace wxgui {

    static float get_clean_num(float num)
    {
      if(num == 0)
        return 0;
      /* extract sign and exponent from num */
      int sign = (num < 0) ? -1 : 1; num = fabs(num);
      float exponent = floor(log10(num));
      /* search for closest number with base 1, 2, 5, 10 */
      float closest_num = 10*pow(10, exponent);
      if(fabs(num - 1*pow(10, exponent)) < fabs(num - closest_num))
        closest_num = 1*pow(10, exponent);
      if(fabs(num - 2*pow(10, exponent)) < fabs(num - closest_num))
        closest_num = 2*pow(10, exponent);
      if(fabs(num - 5*pow(10, exponent)) < fabs(num - closest_num))
        closest_num = 5*pow(10, exponent);
      return sign*closest_num;
    }

    histo_sink_f::sptr
    histo_sink_f::make(msg_queue::sptr msgq)
    {
      return gnuradio::get_initial_sptr
        (new histo_sink_f_impl(msgq));
    }

    histo_sink_f_impl::histo_sink_f_impl(msg_queue::sptr msgq)
      : sync_block("histo_sink_f",
                      io_signature::make(1, 1, sizeof(float)),
                      io_signature::make(0, 0, 0)),
        d_msgq(msgq), d_num_bins(11), d_frame_size(1000),
        d_sample_count(0), d_bins(NULL), d_samps(NULL)
    {
      //allocate arrays and clear
      set_num_bins(d_num_bins);
      set_frame_size(d_frame_size);
    }

    histo_sink_f_impl::~histo_sink_f_impl(void)
    {
      delete [] d_samps;
      delete [] d_bins;
    }

    int
    histo_sink_f_impl::work(int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
    {
      const float *in = (const float*)input_items[0];
      gr::thread::scoped_lock guard(d_mutex);  // hold mutex for duration of this function
      for(unsigned int i = 0; i < (unsigned int)noutput_items; i++) {
        d_samps[d_sample_count] = in[i];
        d_sample_count++;
        /* processed a frame? */
        if(d_sample_count == d_frame_size) {
          send_frame();
          clear();
        }
      }
      return noutput_items;
    }

    void
    histo_sink_f_impl::send_frame(void)
    {
      /* output queue full, drop the data */
      if(d_msgq->full_p())
        return;
      /* find the minimum and maximum */
      float minimum = d_samps[0];
      float maximum = d_samps[0];
      for(unsigned int i = 0; i < d_frame_size; i++) {
        if(d_samps[i] < minimum) minimum = d_samps[i];
        if(d_samps[i] > maximum) maximum = d_samps[i];
      }
      minimum = get_clean_num(minimum);
      maximum = get_clean_num(maximum);
      if(minimum == maximum || minimum > maximum)
        return; //useless data or screw up?
      /* load the bins */
      int index;
      float bin_width = (maximum - minimum)/(d_num_bins-1);
      for(unsigned int i = 0; i < d_sample_count; i++) {
        index = boost::math::iround((d_samps[i] - minimum)/bin_width);
        /* ensure the index range in case a small floating point error is involed */
        if(index < 0)
          index = 0;
        if(index >= (int)d_num_bins)
          index = d_num_bins-1;
        d_bins[index]++;
      }
      /* Build a message to hold the output records */
      message::sptr msg = message::make(0, minimum, maximum, d_num_bins*sizeof(float));
      float *out = (float *)msg->msg(); // get pointer to raw message buffer
      /* normalize the bins and put into message */
      for(unsigned int i = 0; i < d_num_bins; i++) {
        out[i] = ((float)d_bins[i])/d_frame_size;
      }
      /* send the message */
      d_msgq->handle(msg);
    }

    void
    histo_sink_f_impl::clear(void)
    {
      d_sample_count = 0;
      /* zero the bins */
      for(unsigned int i = 0; i < d_num_bins; i++) {
        d_bins[i] = 0;
      }
    }

    /**************************************************
     * Getters
     **************************************************/
    unsigned int
    histo_sink_f_impl::get_frame_size(void)
    {
      return d_frame_size;
    }

    unsigned int
    histo_sink_f_impl::get_num_bins(void)
    {
      return d_num_bins;
    }

    /**************************************************
     * Setters
     **************************************************/
    void
    histo_sink_f_impl::set_frame_size(unsigned int frame_size)
    {
      gr::thread::scoped_lock guard(d_mutex);    // hold mutex for duration of this function
      d_frame_size = frame_size;
      /* allocate a new sample array */
      delete [] d_samps;
      d_samps = new float[d_frame_size];
      clear();
    }

    void
    histo_sink_f_impl::set_num_bins(unsigned int num_bins) {
      gr::thread::scoped_lock guard(d_mutex);    // hold mutex for duration of this function
      d_num_bins = num_bins;
      /* allocate a new bin array */
      delete [] d_bins;
      d_bins = new unsigned int[d_num_bins];
      clear();
    }

  } /* namespace wxgui */
} /* namespace gr */
