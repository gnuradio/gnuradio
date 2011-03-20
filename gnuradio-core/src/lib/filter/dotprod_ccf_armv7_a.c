/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2011 Free Software Foundation, Inc.
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

#include <dotprod_ccf_armv7_a.h>

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

void
dotprod_ccf_armv7_a(const float *a, const float *b, float *res, size_t n)
{
  size_t i;
  res[0] = 0;
  res[1] = 0;

  for (i = 0; i < n; i++){
    res[0] += a[2*i] * b[i];
    res[1] += a[2*i+1] * b[i];
  }
}

#else

/*
 *  preconditions:
 *
 *    n > 0 and a multiple of 4
 *    a   4-byte aligned
 *    b  16-byte aligned
 */
void
dotprod_ccf_armv7_a(const float *a, const float *b, float *res, size_t n)
{

        asm volatile(
		"vmov.f32       q14, #0.0               \n\t"
		"vmov.f32       q15, #0.0               \n\t"
		"1:                                     \n\t"
		"subs           %2, %2, #4              \n\t"
		"vld2.f32       {q0-q1}, [%0]!          \n\t"
		"vld1.f32       {q2}, [%1]!             \n\t"
		"vmla.f32       q14, q0, q2             \n\t"
		"vmla.f32       q15, q1, q2             \n\t"
		"bgt            1b                      \n\t"
		"vpadd.f32      d0, d28, d29            \n\t"
		"vpadd.f32      d1, d30, d31            \n\t"
		"vpadd.f32	d0, d0, d1              \n\t"
		"vst1.f32       {d0}, [%3]              \n\t"

		: "+&r"(a), "+&r"(b), "+&r"(n)
		: "r"(res)
		: "memory", "d0", "d1", "d2", "d3", "d4", "d5",
		   "d28", "d29", "d30", "d31" );
}


#endif
