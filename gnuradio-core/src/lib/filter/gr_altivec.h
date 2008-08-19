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

#include <altivec.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VS             sizeof(vector float)
#define FLOATS_PER_VEC (sizeof(vector float)/sizeof(float))

union v_float_u {
  vector float	v;
  float		f[FLOATS_PER_VEC];
};

void gr_print_vector_float(FILE *fp, vector float v);
void gr_pvf(FILE *fp, const char *label, vector float v);

static inline float
horizontal_add_f(vector float v)
{
  union v_float_u u;
  vector float	  t0 = vec_add(v, vec_sld(v, v, 8));
  vector float	  t1 = vec_add(t0, vec_sld(t0, t0, 4));
  u.v = t1;
  return u.f[0];
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_GR_ALTIVEC_H */
