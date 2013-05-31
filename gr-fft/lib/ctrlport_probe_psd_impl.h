/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CTRLPORT_PROBE_PSD_IMPL_H
#define INCLUDED_CTRLPORT_PROBE_PSD_IMPL_H

#include <gnuradio/fft/ctrlport_probe_psd.h>
#include <gnuradio/rpcregisterhelpers.h>
#include <boost/thread/shared_mutex.hpp>
#include <gnuradio/fft/fft.h>

namespace gr {
  namespace fft {

    class ctrlport_probe_psd_impl : public ctrlport_probe_psd
    {
    private:
      std::string d_id;
      std::string d_desc;
      size_t d_len;
      boost::shared_mutex mutex_buffer;
      mutable boost::mutex mutex_notify;
      boost::condition_variable condition_buffer_ready;

      std::vector<gr_complex> d_buffer;
      gr::fft::fft_complex d_fft;

    public:
      ctrlport_probe_psd_impl(const std::string &id, const std::string &desc, int len);
      ~ctrlport_probe_psd_impl();

      void setup_rpc();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      std::vector<gr_complex> get();

      void set_length(int len);
      int length() const;

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_PROBE_PSD_IMPL_H */

