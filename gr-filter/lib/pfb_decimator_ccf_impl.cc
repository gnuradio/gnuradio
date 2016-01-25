/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012,2014 Free Software Foundation, Inc.
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

#include "pfb_decimator_ccf_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/expj.h>
#include <volk/volk.h>

namespace gr {
  namespace filter {

    pfb_decimator_ccf::sptr
    pfb_decimator_ccf::make(unsigned int decim,
			    const std::vector<float> &taps,
			    unsigned int channel,
                            bool use_fft_rotator,
                            bool use_fft_filters)
    {
      return gnuradio::get_initial_sptr
	(new pfb_decimator_ccf_impl(decim, taps, channel,
                                    use_fft_rotator,
                                    use_fft_filters));
    }

    pfb_decimator_ccf_impl::pfb_decimator_ccf_impl(unsigned int decim,
						   const std::vector<float> &taps,
						   unsigned int channel,
                                                   bool use_fft_rotator,
                                                   bool use_fft_filters)
      : sync_block("pfb_decimator_ccf",
                   io_signature::make(decim, decim, sizeof(gr_complex)),
                   io_signature::make(1, 1, sizeof(gr_complex))),
	polyphase_filterbank(decim, taps),
	d_updated(false), d_chan(channel),
        d_use_fft_rotator(use_fft_rotator),
        d_use_fft_filters(use_fft_filters)
    {
      d_rate = decim;
      d_rotator = new gr_complex[d_rate];
      for(unsigned int i = 0; i < d_rate; i++) {
        d_rotator[i] = gr_expj(i*d_chan*2*M_PI/d_rate);
      }

      set_relative_rate(1.0/(float)decim);

      if(d_use_fft_filters) {
        set_history(1);
        set_output_multiple(d_fft_filters[0]->filtersize() - d_fft_filters[0]->ntaps() + 1);
      }
      else {
        set_history(d_taps_per_filter);
      }

      d_tmp = NULL;
    }

    bool pfb_decimator_ccf_impl::start()
    {
      if(d_use_fft_filters) {
        d_tmp = fft::malloc_complex(max_noutput_items()*d_rate);
      }

      return block::start();
    }

    bool pfb_decimator_ccf_impl::stop()
    {
      if((d_use_fft_filters) && (d_tmp)) {
        fft::free(d_tmp);
      }

      return block::stop();
    }

    pfb_decimator_ccf_impl::~pfb_decimator_ccf_impl()
    {
      delete d_rotator;
    }

    void
    pfb_decimator_ccf_impl::set_taps(const std::vector<float> &taps)
    {
      gr::thread::scoped_lock guard(d_mutex);

      polyphase_filterbank::set_taps(taps);
      set_history(d_taps_per_filter);
      d_updated = true;
    }

    void
    pfb_decimator_ccf_impl::print_taps()
    {
      polyphase_filterbank::print_taps();
    }

    std::vector<std::vector<float> >
    pfb_decimator_ccf_impl::taps() const
    {
      return polyphase_filterbank::taps();
    }

    void
    pfb_decimator_ccf_impl::set_channel(const unsigned int chan)
    {
        gr::thread::scoped_lock guard(d_mutex);
        d_chan = chan;
    }

    int
    pfb_decimator_ccf_impl::work(int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_mutex);

      if(d_updated) {
	d_updated = false;
	return 0;		     // history requirements may have changed.
      }

      if(d_use_fft_rotator) {
        if(d_use_fft_filters) {
          return work_fft_fft(noutput_items, input_items, output_items);
        }
        else {
          return work_fir_fft(noutput_items, input_items, output_items);
        }
      }
      else {
        if(d_use_fft_filters) {
          return work_fft_exp(noutput_items, input_items, output_items);
        }
        else {
          return work_fir_exp(noutput_items, input_items, output_items);
        }
      }
    }

    int
    pfb_decimator_ccf_impl::work_fir_exp(int noutput_items,
                                         gr_vector_const_void_star &input_items,
                                         gr_vector_void_star &output_items)
    {
      gr_complex *in;
      gr_complex *out = (gr_complex *)output_items[0];

      int i;
      for(i = 0; i < noutput_items; i++) {
	// Move through filters from bottom to top
	out[i] = 0;
	for(int j = d_rate-1; j >= 0; j--) {
	  // Take items from M-1 to 0; filter and rotate
	  in = (gr_complex*)input_items[d_rate - 1 - j];
          out[i] += d_fir_filters[j]->filter(&in[i])*d_rotator[j];
	}
      }

      return noutput_items;
    }

    int
    pfb_decimator_ccf_impl::work_fir_fft(int noutput_items,
                                         gr_vector_const_void_star &input_items,
                                         gr_vector_void_star &output_items)
    {
      gr_complex *in;
      gr_complex *out = (gr_complex *)output_items[0];

      int i;
      for(i = 0; i < noutput_items; i++) {
	// Move through filters from bottom to top
	out[i] = 0;
	for(unsigned int j = 0; j < d_rate; j++) {
	  // Take in the items from the first input stream to d_rate
	  in = (gr_complex*)input_items[d_rate-1-j];
          d_fft->get_inbuf()[j] = d_fir_filters[j]->filter(&in[i]);
	}

        // Perform the FFT to do the complex multiply despinning for all channels
        d_fft->execute();

        // Select only the desired channel out
        out[i] = d_fft->get_outbuf()[d_chan];
      }

      return noutput_items;
    }

    int
    pfb_decimator_ccf_impl::work_fft_exp(int noutput_items,
                                         gr_vector_const_void_star &input_items,
                                         gr_vector_void_star &output_items)
    {
      gr_complex *in;
      gr_complex *out = (gr_complex *)output_items[0];

      int i;

      // Filter each input stream by the FFT filters; do all
      // noutput_items at once to avoid repeated calls to the FFT
      // setup and operation.
      for(unsigned int j = 0; j < d_rate; j++) {
        in = (gr_complex*)input_items[d_rate-j-1];
        d_fft_filters[j]->filter(noutput_items, in, &(d_tmp[j*noutput_items]));
      }

      // Rotate and add filter outputs (k=channel number; M=number of
      // channels; and x[j] is the output of filter j.
      //   y[i] = \sum_{j=0}{M-1} (x[j][i]*exp(2j \pi j k / M))
      for(i = 0; i < noutput_items; i++) {
        out[i] = 0;
        for(unsigned int j = 0; j < d_rate; j++) {
          out[i] += d_tmp[j*noutput_items+i]*d_rotator[j];
        }
      }

      return noutput_items;
    }

    int
    pfb_decimator_ccf_impl::work_fft_fft(int noutput_items,
                                         gr_vector_const_void_star &input_items,
                                         gr_vector_void_star &output_items)
    {
      gr_complex *in;
      gr_complex *out = (gr_complex *)output_items[0];

      int i;

      for(unsigned int j = 0; j < d_rate; j++) {
        in = (gr_complex*)input_items[d_rate-j-1];
        d_fft_filters[j]->filter(noutput_items, in, &d_tmp[j*noutput_items]);
      }

      // Performs the rotate and add operations by implementing it as
      // an FFT.
      for(i = 0; i < noutput_items; i++) {
        for(unsigned int j = 0; j < d_rate; j++) {
          d_fft->get_inbuf()[j] = d_tmp[j*noutput_items + i];
        }

        // Perform the FFT to do the complex multiply despinning for all channels
        d_fft->execute();

        // Select only the desired channel out
        out[i] = d_fft->get_outbuf()[d_chan];
      }

      return noutput_items;
    }


  } /* namespace filter */
} /* namespace gr */
