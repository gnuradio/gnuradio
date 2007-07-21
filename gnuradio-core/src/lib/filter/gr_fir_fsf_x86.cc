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
#include <gr_fir_fsf_x86.h>
#include <float_dotprod_x86.h>

/*
 * 	--- 3DNow! version ---
 */
 
gr_fir_fsf_3dnow::gr_fir_fsf_3dnow ()
  : gr_fir_fsf_simd () 
{
  d_float_dotprod = float_dotprod_3dnow;
}

gr_fir_fsf_3dnow::gr_fir_fsf_3dnow (const std::vector<float> &new_taps)
  : gr_fir_fsf_simd (new_taps)
{
  d_float_dotprod = float_dotprod_3dnow;
}


/*
 * 	--- SSE version ---
 */
 
gr_fir_fsf_sse::gr_fir_fsf_sse ()
  : gr_fir_fsf_simd ()
{
  d_float_dotprod = float_dotprod_sse;
}

gr_fir_fsf_sse::gr_fir_fsf_sse (const std::vector<float> &new_taps)
  : gr_fir_fsf_simd (new_taps)
{
  d_float_dotprod = float_dotprod_sse;
}
