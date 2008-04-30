/* -*- c++ -*- */
/*
 * Copyright 2004,2008 Free Software Foundation, Inc.
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

#include <gr_complex_to_xxx.h>
#include <gr_io_signature.h>
#include <gr_math.h>

// ----------------------------------------------------------------

gr_complex_to_float_sptr
gr_make_complex_to_float (unsigned int vlen)
{
  return gr_complex_to_float_sptr (new gr_complex_to_float (vlen));
}

gr_complex_to_float::gr_complex_to_float (unsigned int vlen)
  : gr_sync_block ("complex_to_float",
		   gr_make_io_signature (1, 1, sizeof (gr_complex) * vlen),
		   gr_make_io_signature (1, 2, sizeof (float) * vlen)),
    d_vlen(vlen)
{
}

int
gr_complex_to_float::work (int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  float *out0 = (float *) output_items[0];
  float* out1;
  int noi = noutput_items * d_vlen;

  switch (output_items.size ()){
  case 1:
    for (int i = 0; i < noi; i++){
      out0[i] = in[i].real ();
    }
    break;

  case 2:
    out1 = (float *) output_items[1];
    for (int i = 0; i < noi; i++){
      out0[i] = in[i].real ();
      out1[i] = in[i].imag ();
    }
    break;

  default:
    abort ();
  }

  return noutput_items;
}

// ----------------------------------------------------------------

gr_complex_to_real_sptr
gr_make_complex_to_real (unsigned int vlen)
{
  return gr_complex_to_real_sptr (new gr_complex_to_real (vlen));
}

gr_complex_to_real::gr_complex_to_real (unsigned int vlen)
  : gr_sync_block ("complex_to_real",
		   gr_make_io_signature (1, 1, sizeof (gr_complex) * vlen),
		   gr_make_io_signature (1, 1, sizeof (float) * vlen)),
    d_vlen(vlen)
{
}

int
gr_complex_to_real::work (int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  float *out = (float *) output_items[0];
  int noi = noutput_items * d_vlen;

  for (int i = 0; i < noi; i++){
    out[i] = in[i].real ();
  }
  return noutput_items;
}

// ----------------------------------------------------------------

gr_complex_to_imag_sptr
gr_make_complex_to_imag (unsigned int vlen)
{
  return gr_complex_to_imag_sptr (new gr_complex_to_imag (vlen));
}

gr_complex_to_imag::gr_complex_to_imag (unsigned int vlen)
  : gr_sync_block ("complex_to_imag",
		   gr_make_io_signature (1, 1, sizeof (gr_complex) * vlen),
		   gr_make_io_signature (1, 1, sizeof (float) * vlen)),
    d_vlen(vlen)
{
}

int
gr_complex_to_imag::work (int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  float *out = (float *) output_items[0];
  int noi = noutput_items * d_vlen;

  for (int i = 0; i < noi; i++){
    out[i] = in[i].imag ();
  }
  return noutput_items;
}

// ----------------------------------------------------------------

gr_complex_to_mag_sptr
gr_make_complex_to_mag (unsigned int vlen)
{
  return gr_complex_to_mag_sptr (new gr_complex_to_mag (vlen));
}

gr_complex_to_mag::gr_complex_to_mag (unsigned int vlen)
  : gr_sync_block ("complex_to_mag",
		   gr_make_io_signature (1, 1, sizeof (gr_complex) * vlen),
		   gr_make_io_signature (1, 1, sizeof (float) * vlen)),
    d_vlen(vlen)
{
}

int
gr_complex_to_mag::work (int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  float *out = (float *) output_items[0];
  int noi = noutput_items * d_vlen;

  for (int i = 0; i < noi; i++){
    out[i] = std::abs (in[i]);
  }
  return noutput_items;
}

// ----------------------------------------------------------------

gr_complex_to_mag_squared_sptr
gr_make_complex_to_mag_squared (unsigned int vlen)
{
  return gr_complex_to_mag_squared_sptr (new gr_complex_to_mag_squared (vlen));
}

gr_complex_to_mag_squared::gr_complex_to_mag_squared (unsigned int vlen)
  : gr_sync_block ("complex_to_mag_squared",
		   gr_make_io_signature (1, 1, sizeof (gr_complex) * vlen),
		   gr_make_io_signature (1, 1, sizeof (float) * vlen)),
    d_vlen(vlen)
{
}

int
gr_complex_to_mag_squared::work (int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  float *out = (float *) output_items[0];
  int noi = noutput_items * d_vlen;

  for (int i = 0; i < noi; i++){
    const float __x = in[i].real();
    const float __y = in[i].imag();
    out[i] = __x * __x + __y * __y;
  }
  return noutput_items;
}

// ----------------------------------------------------------------

gr_complex_to_arg_sptr
gr_make_complex_to_arg (unsigned int vlen)
{
  return gr_complex_to_arg_sptr (new gr_complex_to_arg (vlen));
}

gr_complex_to_arg::gr_complex_to_arg (unsigned int vlen)
  : gr_sync_block ("complex_to_arg",
		   gr_make_io_signature (1, 1, sizeof (gr_complex) * vlen),
		   gr_make_io_signature (1, 1, sizeof (float) * vlen)),
    d_vlen(vlen)
{
}

int
gr_complex_to_arg::work (int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  float *out = (float *) output_items[0];
  int noi = noutput_items * d_vlen;

  for (int i = 0; i < noi; i++){
    //    out[i] = std::arg (in[i]);
    out[i] = gr_fast_atan2f(in[i]);
  }
  return noutput_items;
}
