/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <gcell/spu/libfft.h>
#include <gcell/spu/fft_1d_r2.h>
#include <assert.h>

/*
 * invoke the inline version
 */
void 
fft_1d_r2(vector float *out, vector float *in, vector float *W, int log2_size)
{
  assert((1 << log2_size) <= MAX_FFT_1D_SIZE);

  _fft_1d_r2(out, in, W, log2_size);
}
