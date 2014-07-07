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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ctrlport_probe_psd_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace fft {

    ctrlport_probe_psd::sptr
    ctrlport_probe_psd::make(const std::string &id,
                            const std::string &desc, int len)
    {
      return gnuradio::get_initial_sptr
        (new ctrlport_probe_psd_impl(id, desc, len));
    }

    ctrlport_probe_psd_impl::ctrlport_probe_psd_impl(const std::string &id,
                                                   const std::string &desc, int len)
      : gr::sync_block("probe_psd",
                      gr::io_signature::make(1, 1, sizeof(gr_complex)),
                      gr::io_signature::make(0, 0, 0)),
        d_id(id), d_desc(desc), d_len(len),
        d_fft(len, true, 1)
    {
      set_length(len);
    }

    ctrlport_probe_psd_impl::~ctrlport_probe_psd_impl()
    {
    }

    void
    ctrlport_probe_psd_impl::forecast(int noutput_items,
                                     gr_vector_int &ninput_items_required)
    {
      // make sure all inputs have noutput_items available
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = d_len;
    }

    //    boost::shared_mutex mutex_buffer;
    //    mutable boost::mutex mutex_notify;
    //    boost::condition_variable condition_buffer_ready;
    std::vector<gr_complex>
    ctrlport_probe_psd_impl::get()
    {
      mutex_buffer.lock();
      d_buffer.clear();
      mutex_buffer.unlock();

      // wait for condition
      boost::mutex::scoped_lock lock(mutex_notify);
      condition_buffer_ready.wait(lock);

      mutex_buffer.lock();

      memcpy(d_fft.get_inbuf(), &d_buffer[0], d_len*sizeof(gr_complex));
      d_fft.execute();
      std::vector<gr_complex> buf_copy;

      buf_copy.resize(d_len);

      gr_complex* out = d_fft.get_outbuf();
      for(size_t i=0; i<d_len; i++){
        size_t idx = (i + d_len/2)%d_len;
        float x = i/(d_len-1.0f)-0.5;
        buf_copy[i] = gr_complex(x, 10*log10( (out[idx]*std::conj(out[idx])).real() ) );
        }
      mutex_buffer.unlock();
      return buf_copy;
    }

    void
    ctrlport_probe_psd_impl::set_length(int len)
    {
      if(len > 8191) {
        std::cerr << "probe_psd: length " << len
                  << " exceeds maximum buffer size of 8191" << std::endl;
        len = 8191;
      }

      d_len = len;
      d_buffer.reserve(d_len);
    }

    int
    ctrlport_probe_psd_impl::length() const
    {
      return (int)d_len;
    }

    int
    ctrlport_probe_psd_impl::work(int noutput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];

      // copy samples to get buffer if we need samples
      mutex_buffer.lock();
      if(d_buffer.size() < d_len) {
        // copy smaller of remaining buffer space and num inputs to work()
        int num_copy = std::min( (int)(d_len - d_buffer.size()), noutput_items );

        // TODO: convert this to a copy operator for speed...
        for(int i = 0; i < num_copy; i++) {
          d_buffer.push_back(in[i]);
        }

        // notify the waiting get() if we fill up the buffer
        if(d_buffer.size() == d_len) {
          condition_buffer_ready.notify_one();
        }
      }
      mutex_buffer.unlock();

      return noutput_items;
    }

    void
    ctrlport_probe_psd_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      int len = static_cast<int>(d_len);
      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_get<ctrlport_probe_psd, std::vector<std::complex<float> > >(
        alias(), d_id.c_str(), &ctrlport_probe_psd::get,
        pmt::make_c32vector(0,-2),
        pmt::make_c32vector(0,2),
        pmt::make_c32vector(0,0),
        "dB", d_desc.c_str(), RPC_PRIVLVL_MIN,
        DISPXY | DISPOPTSCATTER)));

      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_get<ctrlport_probe_psd, int>(
          alias(), "length", &ctrlport_probe_psd::length,
          pmt::mp(1), pmt::mp(10*len), pmt::mp(len),
          "samples", "get vector length", RPC_PRIVLVL_MIN, DISPNULL)));

//      d_rpc_vars.push_back(
//        rpcbasic_sptr(new rpcbasic_register_set<ctrlport_probe_psd, int>(
//          alias(), "length", &ctrlport_probe_psd::set_length,
//          pmt::mp(1), pmt::mp(10*len), pmt::mp(len),
//          "samples", "set vector length", RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace fft */
} /* namespace gr */
