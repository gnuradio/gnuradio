/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <filter/fir_filter_with_buffer.h>
#include <fft/fft.h>
#include <volk/volk.h>

namespace gr {
  namespace filter {
    namespace kernel {

      fir_filter_with_buffer_fff::fir_filter_with_buffer_fff(const std::vector<float> &taps)
      {
	d_buffer = NULL;
	set_taps(taps);
      }

      fir_filter_with_buffer_fff::~fir_filter_with_buffer_fff()
      {
	if(d_buffer != NULL)
	  fft::free(d_buffer);
      }

      void
      fir_filter_with_buffer_fff::set_taps(const std::vector<float> &taps)
      {
	if(d_buffer != NULL) {
	  fft::free(d_buffer);
	  d_buffer = NULL;
	}

	d_ntaps = (int)taps.size();
	d_taps = fft::malloc_float(d_ntaps);
	for(unsigned int i = 0; i < d_ntaps; i++) {
	  d_taps[d_ntaps-i-1] = taps[i];
	}
	d_idx = 0;
      }

      std::vector<float>
      fir_filter_with_buffer_fff::taps() const
      {
	std::vector<float> t;
	for(unsigned int i = 0; i < d_ntaps; i++)
	  t.push_back(d_taps[d_ntaps-i-1]);
	return t;
      }

      float
      fir_filter_with_buffer_fff::filter(float input)
      {
	d_buffer[d_idx] = input;
	d_buffer[d_idx+ntaps()] = input;

	d_idx++;
	if(d_idx >= ntaps())
	  d_idx = 0;

	float output = 0;
	volk_32f_x2_dot_prod_32f_u(&output, &d_buffer[d_idx],
				   d_taps, d_ntaps);

	return output;
      }

      float
      fir_filter_with_buffer_fff::filter(const float input[],
					 unsigned long dec)
      {
	unsigned int i;

	for(i = 0; i < dec; i++) {
	  d_buffer[d_idx] = input[i];
	  d_buffer[d_idx+ntaps()] = input[i];
	  d_idx++;
	  if(d_idx >= ntaps())
	    d_idx = 0;
	}

	float output = 0;
	volk_32f_x2_dot_prod_32f_u(&output, &d_buffer[d_idx],
				   d_taps, d_ntaps);
	return output;
      }

      void
      fir_filter_with_buffer_fff::filterN(float output[],
					  const float input[],
					  unsigned long n)
      {
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(input[i]);
	}
      }

      void
      fir_filter_with_buffer_fff::filterNdec(float output[],
					     const float input[],
					     unsigned long n,
					     unsigned long decimate)
      {
	unsigned long j = 0;
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(&input[j], decimate);
	  j += decimate;
	}
      }

      
      /**************************************************************/


      fir_filter_with_buffer_ccc::fir_filter_with_buffer_ccc(const std::vector<gr_complex> &taps)
      {
	d_buffer = NULL;
	set_taps(taps);
      }

      fir_filter_with_buffer_ccc::~fir_filter_with_buffer_ccc()
      {
	if(d_buffer != NULL)
	  fft::free(d_buffer);
      }

      void
      fir_filter_with_buffer_ccc::set_taps(const std::vector<gr_complex> &taps)
      {
	if(d_buffer != NULL) {
	  fft::free(d_buffer);
	  d_buffer = NULL;
	}

	d_ntaps = (int)taps.size();
	d_taps = fft::malloc_complex(d_ntaps);
	for(unsigned int i = 0; i < d_ntaps; i++) {
	  d_taps[d_ntaps-i-1] = taps[i];
	}
	d_idx = 0;
      }

      std::vector<gr_complex>
      fir_filter_with_buffer_ccc::taps() const
      {
	std::vector<gr_complex> t;
	for(unsigned int i = 0; i < d_ntaps; i++)
	  t.push_back(d_taps[d_ntaps-i-1]);
	return t;
      }

      gr_complex
      fir_filter_with_buffer_ccc::filter(gr_complex input)
      {
	d_buffer[d_idx] = input;
	d_buffer[d_idx+ntaps()] = input;

	d_idx++;
	if(d_idx >= ntaps())
	  d_idx = 0;

	gr_complex output = 0;
	volk_32fc_x2_dot_prod_32fc_u(&output, &d_buffer[d_idx],
				     d_taps, d_ntaps);

	return output;
      }

      gr_complex
      fir_filter_with_buffer_ccc::filter(const gr_complex input[],
					 unsigned long dec)
      {
	unsigned int i;

	for(i = 0; i < dec; i++) {
	  d_buffer[d_idx] = input[i];
	  d_buffer[d_idx+ntaps()] = input[i];
	  d_idx++;
	  if(d_idx >= ntaps())
	    d_idx = 0;
	}

	gr_complex output = 0;
	volk_32fc_x2_dot_prod_32fc_u(&output, &d_buffer[d_idx],
				     d_taps, d_ntaps);
	return output;
      }

      void
      fir_filter_with_buffer_ccc::filterN(gr_complex output[],
					  const gr_complex input[],
					  unsigned long n)
      {
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(input[i]);
	}
      }

      void
      fir_filter_with_buffer_ccc::filterNdec(gr_complex output[],
					     const gr_complex input[],
					     unsigned long n,
					     unsigned long decimate)
      {
	unsigned long j = 0;
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(&input[j], decimate);
	  j += decimate;
	}
      }


    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */
