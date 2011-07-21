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
#ifndef INCLUDED_GR_ALTIVEC_H
#define INCLUDED_GR_ALTIVEC_H

/*
 * N.B., always use "vec_float4" et al. instead of "vector float" to
 * ensure portability across the various powerpc compilers.  Some of
 * them treat "vector" as a context specific keyword, some don't.
 * Avoid the problem by always using the defines in vec_types.h
 * (included below)
 */

#include <gr_core_api.h>
#include <altivec.h>
#undef bool		// repair namespace pollution
#undef vector		// repair namespace pollution

#ifdef HAVE_VEC_TYPES_H
#include <vec_types.h>		// use system version if we've got it
#else
#include <gr_vec_types.h>	// fall back to our local copy
#endif
#undef qword		// repair namespace pollution

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VS             sizeof(vec_float4)
#define FLOATS_PER_VEC (sizeof(vec_float4)/sizeof(float))

union v_float_u {
  vec_float4	v;
  float		f[FLOATS_PER_VEC];
};

GR_CORE_API void gr_print_vector_float(FILE *fp, vec_float4 v);
GR_CORE_API void gr_pvf(FILE *fp, const char *label, vec_float4 v);

static inline float
horizontal_add_f(vec_float4 v)
{
  union v_float_u u;
  vec_float4	  t0 = vec_add(v, vec_sld(v, v, 8));
  vec_float4	  t1 = vec_add(t0, vec_sld(t0, t0, 4));
  u.v = t1;
  return u.f[0];
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_GR_ALTIVEC_H */
