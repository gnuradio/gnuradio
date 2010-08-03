/* -*- c++ -*- */
/*
 * Copyright 2008,2010 Free Software Foundation, Inc.
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

#include <gr_probe_mpsk_snr_c.h>
#include <gr_io_signature.h>

gr_probe_mpsk_snr_c_sptr
gr_make_probe_mpsk_snr_c(double alpha)
{
  return gnuradio::get_initial_sptr(new gr_probe_mpsk_snr_c(alpha));
}

gr_probe_mpsk_snr_c::gr_probe_mpsk_snr_c(double alpha)
  : gr_sync_block ("probe_mpsk_snr_c",
		   gr_make_io_signature(1, 1, sizeof(gr_complex)),
		   gr_make_io_signature(0, 0, 0)),
    d_signal_mean(0.0),
    d_noise_variance(0.0)
{
  set_alpha(alpha);
}

gr_probe_mpsk_snr_c::~gr_probe_mpsk_snr_c()
{
}

int
gr_probe_mpsk_snr_c::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];

  for (int i = 0; i < noutput_items; i++){
    // Update of signal mean estimate
    double mag = abs(in[i]);
    d_signal_mean = d_alpha*abs(in[i]) + d_beta*d_signal_mean;

    // Update noise variance estimate
    double noise = mag-d_signal_mean;
    double var = noise*noise;
    d_noise_variance = d_alpha*var + d_beta*d_noise_variance;
  }

  return noutput_items;
}

double
gr_probe_mpsk_snr_c::snr() const
{
  if (d_noise_variance == 0.0)
    return 0.0;
  else
    return 10*log10(d_signal_mean*d_signal_mean/d_noise_variance);
}

void
gr_probe_mpsk_snr_c::set_alpha(double alpha)
{
  d_alpha = alpha;
  d_beta = 1.0-alpha;
}
