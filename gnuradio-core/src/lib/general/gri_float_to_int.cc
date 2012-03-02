/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#define _ISOC9X_SOURCE
#include <gri_float_to_int.h>
#include <math.h>
#include <stdint.h>

static const int64_t MAX_INT =  2147483647; //  (2^31)-1
static const int64_t MIN_INT = -2147483647; // -(2^31)-1


void 
gri_float_to_int (const float *in, int *out, float scale, int nsamples)
{
  for (int i = 0; i < nsamples; i++){
    int64_t r = llrintf(scale * in[i]);
    if (r < MIN_INT)
      r = MIN_INT;
    else if (r > MAX_INT)
      r = MAX_INT;
    out[i] = static_cast<int>(r);
  }
}
