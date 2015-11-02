/* -*- c++ -*- */
/*
 * Copyright 2002,2012,2014 Free Software Foundation, Inc.
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

#include <gnuradio/filter/iir_filter.h>

namespace gr {
  namespace filter {
    namespace kernel {

      template<>
      gr_complex
      iir_filter<gr_complex, gr_complex, float, gr_complex>::filter(const gr_complex input)
      {
	gr_complex acc;
	unsigned i = 0;
	unsigned n = ntaps_ff();
	unsigned m = ntaps_fb();

	if(n == 0)
	  return (gr_complex)0;

	int latest_n = d_latest_n;
	int latest_m = d_latest_m;

	acc = d_fftaps[0] * input;
	for(i = 1; i < n; i ++)
	  acc += (d_fftaps[i] * d_prev_input[latest_n + i]);
	for(i = 1; i < m; i ++)
	  acc += (d_fbtaps[i] * d_prev_output[latest_m + i]);

	// store the values twice to avoid having to handle wrap-around in the loop
	d_prev_output[latest_m] = acc;
	d_prev_output[latest_m+m] = acc;
	d_prev_input[latest_n] = input;
	d_prev_input[latest_n+n] = input;

	latest_n--;
	latest_m--;
	if(latest_n < 0)
	  latest_n += n;
	if(latest_m < 0)
	  latest_m += m;

	d_latest_m = latest_m;
	d_latest_n = latest_n;
	return static_cast<gr_complex>(acc);
      }

      template<>
      gr_complex
      iir_filter<gr_complex, gr_complex, double, gr_complexd>::filter(const gr_complex input)
      {
	gr_complexd acc;
	unsigned i = 0;
	unsigned n = ntaps_ff();
	unsigned m = ntaps_fb();

	if(n == 0)
	  return (gr_complex)0;

	int latest_n = d_latest_n;
	int latest_m = d_latest_m;

	acc = d_fftaps[0] * static_cast<gr_complexd>(input);
	for(i = 1; i < n; i ++)
	  acc += (d_fftaps[i] * static_cast<gr_complexd>(d_prev_input[latest_n + i]));
	for(i = 1; i < m; i ++)
	  acc += (d_fbtaps[i] * static_cast<gr_complexd>(d_prev_output[latest_m + i]));

	// store the values twice to avoid having to handle wrap-around in the loop
	d_prev_output[latest_m] = acc;
	d_prev_output[latest_m+m] = acc;
	d_prev_input[latest_n] = input;
	d_prev_input[latest_n+n] = input;

	latest_n--;
	latest_m--;
	if(latest_n < 0)
	  latest_n += n;
	if(latest_m < 0)
	  latest_m += m;

	d_latest_m = latest_m;
	d_latest_n = latest_n;
	return static_cast<gr_complex>(acc);
      }

      template<>
      gr_complex
      iir_filter<gr_complex, gr_complex, gr_complexd, gr_complexd>::filter(const gr_complex input)
      {
	gr_complexd acc;
	unsigned i = 0;
	unsigned n = ntaps_ff();
	unsigned m = ntaps_fb();

	if(n == 0)
	  return (gr_complex)0;

	int latest_n = d_latest_n;
	int latest_m = d_latest_m;

	acc = d_fftaps[0] * static_cast<gr_complexd>(input);
	for(i = 1; i < n; i ++)
	  acc += (d_fftaps[i] * static_cast<gr_complexd>(d_prev_input[latest_n + i]));
	for(i = 1; i < m; i ++)
	  acc += (d_fbtaps[i] * static_cast<gr_complexd>(d_prev_output[latest_m + i]));

	// store the values twice to avoid having to handle wrap-around in the loop
	d_prev_output[latest_m] = acc;
	d_prev_output[latest_m+m] = acc;
	d_prev_input[latest_n] = input;
	d_prev_input[latest_n+n] = input;

	latest_n--;
	latest_m--;
	if(latest_n < 0)
	  latest_n += n;
	if(latest_m < 0)
	  latest_m += m;

	d_latest_m = latest_m;
	d_latest_n = latest_n;
	return static_cast<gr_complex>(acc);
      }

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */

