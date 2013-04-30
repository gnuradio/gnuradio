/* -*- c++ -*- */
/*
 * Copyright 2003-2005,2010,2013 Free Software Foundation, Inc.
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

#include "oscope_sink_f_impl.h"
#include <gnuradio/wxgui/oscope_sink_x.h>
#include <gnuradio/wxgui/oscope_guts.h>
#include <gnuradio/io_signature.h>

namespace gr {
  namespace wxgui {

    oscope_sink_f::sptr
    oscope_sink_f::make(double sampling_rate, msg_queue::sptr msgq)
    {
      return gnuradio::get_initial_sptr
        (new oscope_sink_f_impl(sampling_rate, msgq));
    }

    oscope_sink_f_impl::oscope_sink_f_impl(double sampling_rate, msg_queue::sptr msgq)
      : oscope_sink_x("oscope_sink_f",
                      io_signature::make(1, oscope_guts::MAX_CHANNELS,
                                           sizeof(float)),
                      sampling_rate),
        d_msgq(msgq)
    {
      d_guts = new oscope_guts(d_sampling_rate, d_msgq);
    }

    oscope_sink_f_impl::~oscope_sink_f_impl()
    {
    }

    bool
    oscope_sink_f_impl::check_topology(int ninputs, int noutputs)
    {
      return d_guts->set_num_channels(ninputs);
    }

    int
    oscope_sink_f_impl::work(int noutput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items)
    {
      int ni = input_items.size();
      float tmp[oscope_guts::MAX_CHANNELS];

      for(int i = 0; i < noutput_items; i++) {

        // FIXME for now, copy the data.  Fix later if reqd
        for(int ch = 0; ch < ni; ch++)
          tmp[ch] = ((const float*)input_items[ch])[i];

        d_guts->process_sample(tmp);
      }

      return noutput_items;
    }

  } /* namespace wxgui */
} /* namespace gr */
