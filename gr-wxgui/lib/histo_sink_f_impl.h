/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_HISTO_SINK_F_IMPL_H
#define INCLUDED_GR_HISTO_SINK_F_IMPL_H

#include <gnuradio/wxgui/histo_sink_f.h>

namespace gr {
  namespace wxgui {

    class histo_sink_f_impl : public histo_sink_f
    {
    private:
      msg_queue::sptr d_msgq;
      unsigned int d_num_bins;
      unsigned int d_frame_size;
      unsigned int d_sample_count;
      unsigned int *d_bins;
      float *d_samps;
      gr::thread::mutex d_mutex;

      void send_frame(void);
      void clear(void);

    public:
      histo_sink_f_impl(msg_queue::sptr msgq);
      ~histo_sink_f_impl(void);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);

      unsigned int get_frame_size(void);
      unsigned int get_num_bins(void);

      void set_frame_size(unsigned int frame_size);
      void set_num_bins(unsigned int num_bins);
    };

  } /* namespace wxgui */
} /* namespace gr */

#endif /* INCLUDED_GR_HISTO_SINK_F_IMPL_H */
