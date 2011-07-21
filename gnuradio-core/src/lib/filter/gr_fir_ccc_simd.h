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
#ifndef INCLUDED_GR_FIR_CCC_SIMD_H
#define INCLUDED_GR_FIR_CCC_SIMD_H

#include <gr_core_api.h>
#include <gr_fir_ccc_generic.h>

/*!
 * \brief common base class for SIMD versions of gr_fir_ccc
 * \ingroup filter_primitive
 *
 * This base class handles alignment issues common to SSE and 3DNOW
 * subclasses.
 */

class GR_CORE_API gr_fir_ccc_simd : public gr_fir_ccc_generic
{
protected:
  typedef void (*ccomplex_dotprod_t)(const float *input,
				     const float *taps,
				     unsigned n_2_ccomplex_blocks,
				     float *result);

  /*!
   * \p aligned_taps holds 4 copies of the coefficients preshifted
   * by 0, 1, 2, or 3 floats to meet all possible input data alignments.
   * This allows us to always fetch data and taps that are 128-bit aligned.
   */
  float 		*d_aligned_taps[4];

  ccomplex_dotprod_t	d_ccomplex_dotprod; 	// fast dot product primitive

public:

  // CREATORS
  gr_fir_ccc_simd ();
  gr_fir_ccc_simd (const std::vector<gr_complex> &taps);
  ~gr_fir_ccc_simd ();

  // MANIPULATORS
  virtual void set_taps (const std::vector<gr_complex> &taps);
  virtual gr_complex filter (const gr_complex input[]);
};

#endif
