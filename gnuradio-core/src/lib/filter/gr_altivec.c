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

#include <gr_altivec.h>

void
gr_print_vector_float(FILE *fp, vec_float4 v)
{
  union v_float_u	u;
  u.v = v;
  fprintf(fp, "{ %f, %f, %f, %f }", u.f[0], u.f[1], u.f[2], u.f[3]);
}
  
void
gr_pvf(FILE *fp, const char *label, vec_float4 v)
{
  fprintf(fp, "%s = ", label);
  gr_print_vector_float(fp, v);
  putc('\n', fp);
}
