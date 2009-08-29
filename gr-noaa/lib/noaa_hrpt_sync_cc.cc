/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#include <noaa_hrpt_sync_cc.h>
#include <gr_io_signature.h>

inline int signum(float f)
{
  return f >= 0.0 ? 1 : -1;
}

noaa_hrpt_sync_cc_sptr
noaa_make_hrpt_sync_cc(float alpha, float beta, float sps, float max_offset)
{
  return gnuradio::get_initial_sptr(new noaa_hrpt_sync_cc(alpha, beta, sps, max_offset));
}

noaa_hrpt_sync_cc::noaa_hrpt_sync_cc(float alpha, float beta, float sps, float max_offset)
  : gr_block("noaa_hrpt_sync_cc",
	     gr_make_io_signature(1, 1, sizeof(gr_complex)),
	     gr_make_io_signature(1, 1, sizeof(gr_complex))),
    d_alpha(alpha), d_beta(beta), 
    d_sps(sps), d_max_offset(max_offset),
    d_phase(0.0), d_freq(1.0/sps),
    d_last_sign(1)
{
}

int
noaa_hrpt_sync_cc::general_work(int noutput_items,
				 gr_vector_int &ninput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
{
  int ninputs = ninput_items[0];
  const gr_complex *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  int i = 0, j = 0;
  while (i < ninputs && j < noutput_items) {
    float sample = in[i++].imag();
    int sign = signum(sample);
    d_phase += d_freq;

    // Train on zero crossings in center region of symbol
    if (sign != d_last_sign) {
      if (d_phase > 0.25 && d_phase < 0.75) {
	float phase_err = d_phase-0.5;
	d_phase -= phase_err*d_alpha;        // 1st order phase adjustment
	d_freq -= phase_err*d_beta;          // 2nd order frequency adjustment
      }

      d_last_sign = sign;
    }

    if (d_phase > 1.0) {
      out[j++] = in[i];
      d_phase -= 1.0;
    }
  }

  consume_each(i);
  return j;
}
