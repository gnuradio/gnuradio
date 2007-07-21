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

#include "float_dotprod_generic.h"


float 
float_dotprod_generic (const float *input,
		       const float *taps, unsigned n_4_float_blocks)
{
  float sum0 = 0;
  float sum1 = 0;
  float sum2 = 0;
  float sum3 = 0;

  do {

    sum0 += input[0] * taps[0];
    sum1 += input[1] * taps[1];
    sum2 += input[2] * taps[2];
    sum3 += input[3] * taps[3];

    input += 4;
    taps += 4;

  } while (--n_4_float_blocks != 0);


  return sum0 + sum1 + sum2 + sum3;
}
