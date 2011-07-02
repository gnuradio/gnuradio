/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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
#include <gr_fir_scc_generic.h>

#if (2 == 4)

gr_complex
gr_fir_scc_generic::filter (const short input[])
{
  static const int N_UNROLL = 4;

  gr_complex	acc0 = 0;
  gr_complex	acc1 = 0;
  gr_complex	acc2 = 0;
  gr_complex	acc3 = 0;


  unsigned	i = 0;
  unsigned	n = (ntaps () / N_UNROLL) * N_UNROLL;

  for (i = 0; i < n; i += N_UNROLL){
    acc0 += d_taps[i + 0] * (float) input[i + 0];
    acc1 += d_taps[i + 1] * (float) input[i + 1];
    acc2 += d_taps[i + 2] * (float) input[i + 2];
    acc3 += d_taps[i + 3] * (float) input[i + 3];
  }

  for (; i < ntaps (); i++)
    acc0 += d_taps[i] * (float) input[i];

  return (gr_complex) (acc0 + acc1 + acc2 + acc3);
}

#else 

gr_complex
gr_fir_scc_generic::filter (const short input[])
{
  static const int N_UNROLL = 2;

  gr_complex	acc0 = 0;
  gr_complex	acc1 = 0;

  unsigned	i = 0;
  unsigned	n = (ntaps () / N_UNROLL) * N_UNROLL;

  for (i = 0; i < n; i += N_UNROLL){
    acc0 += d_taps[i + 0] * (float) input[i + 0];
    acc1 += d_taps[i + 1] * (float) input[i + 1];
  }

  for (; i < ntaps (); i++)
    acc0 += d_taps[i] * (float) input[i];

  return (gr_complex) (acc0 + acc1);
}

#endif // N_UNROLL

void 
gr_fir_scc_generic::filterN (gr_complex output[],
			     const short input[],
			     unsigned long n)
{
  for (unsigned i = 0; i < n; i++)
    output[i] = filter (&input[i]);
}

void 
gr_fir_scc_generic::filterNdec (gr_complex output[],
				const short input[],
				unsigned long n,
				unsigned decimate)
{
  unsigned j = 0;
  for (unsigned i = 0; i < n; i++){
    output[i] = filter (&input[j]);
    j += decimate;
  }
}
