/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dotprod_fff_armv7_a.h>

/*!
 * \param x any value
 * \param pow2 must be a power of 2
 * \returns \p x rounded down to a multiple of \p pow2.
 */
static inline size_t
gr_p2_round_down(size_t x, size_t pow2)
{
  return x & -pow2;
}


#if 0

float
dotprod_fff_armv7_a(const float *a, const float *b, size_t n)
{
  float	sum = 0;
  size_t i;
  for (i = 0; i < n; i++){
    sum += a[i] * b[i];
  }
  return sum;
}

#else

/*
 *  preconditions:
 *
 *    n > 0 and a multiple of 4
 *    a   4-byte aligned
 *    b  16-byte aligned
 */
float
dotprod_fff_armv7_a(const float *a, const float *b, size_t n)
{
     float s = 0;

    asm ("vmov.f32  q8, #0.0                  \n\t"
         "vmov.f32  q9, #0.0                  \n\t"
         "1:                                  \n\t"
         "subs      %3, %3, #8                \n\t"
         "vld1.32   {d0,d1,d2,d3}, [%1]!      \n\t"
         "vld1.32   {d4,d5,d6,d7}, [%2]!      \n\t"
         "vmla.f32  q8, q0, q2                \n\t"
         "vmla.f32  q9, q1, q3                \n\t"
         "bgt       1b                        \n\t"
         "vadd.f32  q8, q8, q9                \n\t"
         "vpadd.f32 d0, d16, d17              \n\t"
         "vadd.f32  %0, s0, s1                \n\t"
         : "=w"(s), "+r"(a), "+r"(b), "+r"(n)
         :: "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
	 "d16", "d17", "d18", "d19");

    return s;

}

#endif
