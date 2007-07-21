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

#ifndef _CCOMPLEX_DOTPROD_X86_H_
#define _CCOMPLEX_DOTPROD_X86_H_

#ifdef __cplusplus
extern "C" {
#endif

void
ccomplex_dotprod_3dnow (const float *input,
		     const float *taps, unsigned n_2_ccomplex_blocks, float *result);

void
ccomplex_dotprod_3dnowext (const float *input,
		     const float *taps, unsigned n_2_ccomplex_blocks, float *result);

void
ccomplex_dotprod_sse (const float *input,
		   const float *taps, unsigned n_2_ccomplex_blocks, float *result);

#ifdef __cplusplus
}
#endif

#endif /* _CCOMPLEX_DOTPROD_X86_H_ */
