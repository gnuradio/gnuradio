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

#include <gc_declare_proc.h>
#include <libfft.h>

/*
 * v is really vector complex<float>
 */
static void
conjugate_vector(vector float *v, int nelements)
{
  vector float k = {1, -1, 1, -1};
  int i;
  for (i = 0; i < nelements; i++)
    v[i] *= k;
}

static void
gcs_fft_1d_r2(const gc_job_direct_args_t *input,
	      gc_job_direct_args_t *output __attribute__((unused)),
	      const gc_job_ea_args_t *eaa)
{
  vector float *out = (vector float *) eaa->arg[0].ls_addr;
  vector float *in = (vector float *) eaa->arg[1].ls_addr;
  vector float *W = (vector float *) eaa->arg[2].ls_addr;
  int log2_fft_length = input->arg[0].u32;
  int forward = input->arg[1].u32;	// non-zero if forward xform

  if (forward){
    fft_1d_r2(out, in, W, log2_fft_length);
  }
  else {
    conjugate_vector(in, 1 << (log2_fft_length - 1));
    fft_1d_r2(out, in, W, log2_fft_length);
    conjugate_vector(out, 1 << (log2_fft_length - 1));
  }
}

GC_DECLARE_PROC(gcs_fft_1d_r2, "fft_1d_r2");
