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

#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include "msk_correlate_cc_impl.h"
#include <volk/volk.h>
#include <boost/format.hpp>
#include <boost/math/special_functions/round.hpp>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/pfb_arb_resampler.h>

namespace gr {
  namespace digital {

    msk_correlate_cc::sptr
    msk_correlate_cc::make(const std::vector<float> &symbols,
                           float bt,
                           float sps)
    {
      return gnuradio::get_initial_sptr
        (new msk_correlate_cc_impl(symbols, bt, sps));
    }

    msk_correlate_cc_impl::msk_correlate_cc_impl(const std::vector<float> &symbols,
                                                 float bt,
                                                 float sps)
      : sync_block("msk_correlate_cc",
                   io_signature::make(1, 1, sizeof(gr_complex)),
                   io_signature::make(2, 2, sizeof(gr_complex)))
    {
      d_last_index = 0;
      d_sps = sps;
      // Construct a Gaussian symbol filter with the specified BT
      // We use a gain of 1 because the convolution below will integrate
      // to the appropriate magnitude.
      const int nfilts=32;
      std::vector<float> taps1 = firdes::gaussian(1, nfilts, bt, 20*nfilts);
      // Create an integrating rectangular window to convolve with the Gaussian filter
      std::vector<float> taps2(nfilts, 1);
      //convolve the Gaussian symbol filter with the rectangular window
      //taps2 is symmetric so i don't care about reversing
      //this would be so much nicer in python
      std::vector<float> taps(taps1.size()+taps2.size()-1, 0);
      for(unsigned int i=0; i<taps.size(); i++) {
          int idx = i-taps2.size()+1;
          int lb = std::max(0, -idx);
          int ub = std::min(taps2.size(), taps1.size() - idx);
          for(int k=lb; k<ub; k++) {
              taps[i] += taps1[idx+k]*taps2[k];
          }
      }

      filter::kernel::pfb_arb_resampler_fff resamp = filter::kernel::pfb_arb_resampler_fff(sps, taps, nfilts);

      //Make incoming symbols NRZ.
      //It's just really convenient to use the same mapping for bits
      //as everything else does; otherwise the user would have to
      //specify their bits in [1, -1], and it's going to trip users up.
      std::vector<float> nrz_symbols(symbols);
      for(size_t i=0; i<symbols.size(); i++) {
          nrz_symbols[i] = symbols[i] > 0.0f ? 1.0f : -1.0f;
      }

      // We want to add padding to the beginning of the symbols so we
      // can do the convolution of the symbols with the pulse shape.
      std::vector<float> padding((1+taps.size()/nfilts)/2, 0);
      std::vector<float> padded_symbols = nrz_symbols;
      padded_symbols.insert(padded_symbols.begin(), padding.begin(), padding.end());

      int nread;
      std::vector<float> resampled_symbols(symbols.size()*sps, 0);
      resamp.filter(&resampled_symbols[0], &padded_symbols[0], symbols.size(), nread);

      d_symbols.resize(resampled_symbols.size(), 0);
      //phase modulation of the PAM symbols
      float phase = 0;
      float phase_inc = -M_PI/(2*d_sps);
      for(unsigned int i=0; i<resampled_symbols.size(); i++) {
        d_symbols[i] = exp(gr_complex(0,1)*phase);
        phase += phase_inc*resampled_symbols[i];
      }

      std::reverse(d_symbols.begin(), d_symbols.end());

      float corr = 0;
      for(size_t i=0; i < d_symbols.size(); i++)
        corr += abs(d_symbols[i]*conj(d_symbols[i]));
      d_thresh = 0.9*corr*corr;

      d_center_first_symbol = (padding.size() + 0.5) * d_sps;

      d_filter = new kernel::fft_filter_ccc(1, d_symbols);

      set_history(d_filter->ntaps());

      const int alignment_multiple =
        volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));
    }

    msk_correlate_cc_impl::~msk_correlate_cc_impl()
    {
      delete d_filter;
    }

    std::vector<gr_complex>
    msk_correlate_cc_impl::symbols() const
    {
      return d_symbols;
    }

    int
    msk_correlate_cc_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock lock(d_setlock);

      const gr_complex *in = (gr_complex *)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];
      gr_complex *corr = (gr_complex*)output_items[1];

      memcpy(out, in, sizeof(gr_complex)*noutput_items);

      // Calculate the correlation with the known symbol
      d_filter->filter(noutput_items, in, corr);

      // Find the magnitude squared of the correlation
      std::vector<float> corr_mag(noutput_items);
      volk_32fc_magnitude_squared_32f(&corr_mag[0], corr, noutput_items);

      int i = d_sps;
      while(i < noutput_items) {
        if((corr_mag[i] - corr_mag[i-int(d_sps)]) > d_thresh) {
          while(corr_mag[i] < corr_mag[i+1])
            i++;

          double nom = 0, den = 0;
          for(int s = 0; s < 3; s++) {
            nom += (s+1)*corr_mag[i+s-1];
            den += corr_mag[i+s-1];
          }
          double center;
          if(den <= 1e-9) center = 0;
          else center = nom / den;
          center = (center - 2.0);

          int index = i-d_symbols.size()+int(d_sps)+1;

          float phase = fast_atan2f(corr[index].imag(), corr[index].real());
          if(corr[index].real() < 0.0)
            phase += M_PI;
          add_item_tag(0, nitems_written(0) + index, pmt::intern("phase_est"),
                       pmt::from_double(phase), pmt::intern(alias()));
          add_item_tag(0, nitems_written(0) + index, pmt::intern("time_est"),
                       pmt::from_double(center), pmt::intern(alias()));
          add_item_tag(0, nitems_written(0) + index, pmt::intern("corr_est"),
                       pmt::from_double(corr_mag[index]), pmt::intern(alias()));

          i += d_sps;
        }
        else
          i++;
      }

      return noutput_items;
    }

  } /* namespace digital*/
} /* namespace gr */

