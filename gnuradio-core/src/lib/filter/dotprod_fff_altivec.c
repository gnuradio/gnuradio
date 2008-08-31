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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <dotprod_fff_altivec.h>
#include <gr_altivec.h>

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
dotprod_fff_altivec(const float *a, const float *b, size_t n)
{
  float	sum = 0;
  for (size_t i = 0; i < n; i++){
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
dotprod_fff_altivec(const float *_a, const float *_b, size_t n)
{
  const vec_float4 *a = (const vec_float4 *) _a;
  const vec_float4 *b = (const vec_float4 *) _b;

  static const size_t UNROLL_CNT = 4;

  n = gr_p2_round_down(n, 4);
  size_t loop_cnt = n / (UNROLL_CNT * FLOATS_PER_VEC);
  size_t nleft = n % (UNROLL_CNT * FLOATS_PER_VEC);

  // printf("n = %zd, loop_cnt = %zd, nleft = %zd\n", n, loop_cnt, nleft);

  // Used with vperm to build a* from p*
  vec_uchar16 lvsl_a = vec_lvsl(0, _a);

  vec_float4 p0, p1, p2, p3;
  vec_float4 a0, a1, a2, a3;
  vec_float4 b0, b1, b2, b3;
  vec_float4 acc0 = {0, 0, 0, 0};
  vec_float4 acc1 = {0, 0, 0, 0};
  vec_float4 acc2 = {0, 0, 0, 0};
  vec_float4 acc3 = {0, 0, 0, 0};

  // wind in

  p0 = vec_ld(0*VS, a);
  p1 = vec_ld(1*VS, a);
  p2 = vec_ld(2*VS, a);
  p3 = vec_ld(3*VS, a);
  a += UNROLL_CNT;

  a0 = vec_perm(p0, p1, lvsl_a);
  b0 = vec_ld(0*VS, b);
  p0 = vec_ld(0*VS, a);

  size_t i;
  for (i = 0; i < loop_cnt; i++){

    a1 = vec_perm(p1, p2, lvsl_a);
    b1 = vec_ld(1*VS, b);
    p1 = vec_ld(1*VS, a);
    acc0 = vec_madd(a0, b0, acc0);

    a2 = vec_perm(p2, p3, lvsl_a);
    b2 = vec_ld(2*VS, b);
    p2 = vec_ld(2*VS, a);
    acc1 = vec_madd(a1, b1, acc1);

    a3 = vec_perm(p3, p0, lvsl_a);
    b3 = vec_ld(3*VS, b);
    p3 = vec_ld(3*VS, a);
    acc2 = vec_madd(a2, b2, acc2);

    a += UNROLL_CNT;
    b += UNROLL_CNT;

    a0 = vec_perm(p0, p1, lvsl_a);
    b0 = vec_ld(0*VS, b);
    p0 = vec_ld(0*VS, a);
    acc3 = vec_madd(a3, b3, acc3);
  }

  /*
   * The compiler ought to be able to figure out that 0, 4, 8 and 12
   * are the only possible values for nleft.
   */
  switch (nleft){
  case 0:
    break;
    
  case 4:
    acc0 = vec_madd(a0, b0, acc0);
    break;

  case 8:
    a1 = vec_perm(p1, p2, lvsl_a);
    b1 = vec_ld(1*VS, b);
    acc0 = vec_madd(a0, b0, acc0);
    acc1 = vec_madd(a1, b1, acc1);
    break;

  case 12:
    a1 = vec_perm(p1, p2, lvsl_a);
    b1 = vec_ld(1*VS, b);
    acc0 = vec_madd(a0, b0, acc0);
    a2 = vec_perm(p2, p3, lvsl_a);
    b2 = vec_ld(2*VS, b);
    acc1 = vec_madd(a1, b1, acc1);
    acc2 = vec_madd(a2, b2, acc2);
    break;
  }
	    
  acc0 = acc0 + acc1;
  acc2 = acc2 + acc3;
  acc0 = acc0 + acc2;

  return horizontal_add_f(acc0);
}

#endif
