/* -*- c -*- */
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
#include <gr_complex.h>
#include "complex_dotprod_generic.h"


void
complex_dotprod_generic (const short *input,
		       const float *taps, unsigned n_2_complex_blocks,
		       float *result)
{
  gr_complex sum0(0,0);
  gr_complex sum1(0,0);

  do {
    const gr_complex tap0(taps[0], taps[1]);
    const gr_complex tap1(taps[2], taps[3]);

    sum0 += (float)input[0] * tap0;
    sum1 += (float)input[1] * tap1;

    input += 4;
    taps += 8;

  } while (--n_2_complex_blocks != 0);


  sum0 += sum1;
  result[0] = sum0.real();
  result[1] = sum0.imag();
}
