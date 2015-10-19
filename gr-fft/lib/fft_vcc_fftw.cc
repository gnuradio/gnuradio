/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2010,2012 Free Software Foundation, Inc.
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

#include "fft_vcc_fftw.h"
#include <math.h>
#include <string.h>
#include <volk/volk.h>

namespace gr {
  namespace fft {

    fft_vcc::sptr fft_vcc::make(int fft_size, bool forward,
                const std::vector<float> &window,
                bool shift, int nthreads)
    {
      return gnuradio::get_initial_sptr(new fft_vcc_fftw
                    (fft_size, forward, window,
                     shift, nthreads));
    }

    fft_vcc_fftw::fft_vcc_fftw(int fft_size, bool forward,
                   const std::vector<float> &window,
                   bool shift, int nthreads)
      : sync_block("fft_vcc_fftw",
              io_signature::make(1, 1, fft_size * sizeof(gr_complex)),
              io_signature::make(1, 1, fft_size * sizeof(gr_complex))),
    d_fft_size(fft_size), d_forward(forward), d_shift(shift)
    {
      d_fft = new fft_complex(d_fft_size, forward, nthreads);
      if(!set_window(window))
        throw std::runtime_error("fft_vcc: window not the same length as fft_size\n");
    }

    fft_vcc_fftw::~fft_vcc_fftw()
    {
      delete d_fft;
    }

    void
    fft_vcc_fftw::set_nthreads(int n)
    {
      d_fft->set_nthreads(n);
    }

    int
    fft_vcc_fftw::nthreads() const
    {
      return d_fft->nthreads();
    }

    bool
    fft_vcc_fftw::set_window(const std::vector<float> &window)
    {
      if(window.size()==0 || window.size()==d_fft_size) {
    d_window=window;
    return true;
      }
      else
    return false;
    }

    int
    fft_vcc_fftw::work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      unsigned int input_data_size = input_signature()->sizeof_stream_item (0);
      unsigned int output_data_size = output_signature()->sizeof_stream_item (0);

      int count = 0;

      while(count++ < noutput_items) {

      // copy input into optimally aligned buffer
      if(d_window.size()) {
        gr_complex *dst = d_fft->get_inbuf();
        if(!d_forward && d_shift) {
          unsigned int offset = (!d_forward && d_shift)?(d_fft_size/2):0;
          int fft_m_offset = d_fft_size - offset;
          volk_32fc_32f_multiply_32fc(&dst[fft_m_offset], &in[0], &d_window[0], offset);
          volk_32fc_32f_multiply_32fc(&dst[0], &in[offset], &d_window[offset], d_fft_size-offset);
        }
        else {
          volk_32fc_32f_multiply_32fc(&dst[0], in, &d_window[0], d_fft_size);
        }
      }
      else {
        if(!d_forward && d_shift) {  // apply an ifft shift on the data
          gr_complex *dst = d_fft->get_inbuf();
          unsigned int len = (unsigned int)(floor(d_fft_size/2.0)); // half length of complex array
          memcpy(&dst[0], &in[len], sizeof(gr_complex)*(d_fft_size - len));
          memcpy(&dst[d_fft_size - len], &in[0], sizeof(gr_complex)*len);
        }
        else {
          memcpy(d_fft->get_inbuf(), in, input_data_size);
        }
      }

      // compute the fft
      d_fft->execute();

      // copy result to our output
      if(d_forward && d_shift) {  // apply a fft shift on the data
        unsigned int len = (unsigned int)(ceil(d_fft_size/2.0));
        memcpy(&out[0], &d_fft->get_outbuf()[len], sizeof(gr_complex)*(d_fft_size - len));
        memcpy(&out[d_fft_size - len], &d_fft->get_outbuf()[0], sizeof(gr_complex)*len);
      }
      else {
        memcpy (out, d_fft->get_outbuf (), output_data_size);
      }

      in  += d_fft_size;
      out += d_fft_size;
      }

      return noutput_items;
    }

  } /* namespace fft */
} /* namespace gr */
