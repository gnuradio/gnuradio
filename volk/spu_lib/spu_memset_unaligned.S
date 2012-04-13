/* -*- asm -*- */
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

#include "gc_spu_macs.h"

	.file "spu_memset_unaligned.S"

	/*
	 * Computes this, only a lot faster...
	 *
	 *	void *
	 *	libvector_memset_unaligned(void *pv, int c, size_t n)
	 *	{
	 *	  unsigned char *p = (unsigned char *) pv;
	 *	  size_t i;
	 *	  for (i = 0; i < n; i++)
	 *	    p[i] = c;
	 *
	 *	  return pv;
	 *	}
	 */

#define	p_arg	arg1	// we're going to clobber arg1 w/ the return value
#define	c	arg2	// the constant we're writing
#define	n	arg3	// how many bytes to write

#define	p	r13	// where we're writing
#define	t0	r14
#define t1	r15
#define	mask	r16
#define	old	r17
#define an	r18	// aligned n (n rounded down to mod 16 boundary)
#define	next_p	r19
#define	cond1	r20
#define	cond2	r21
#define m	r22
#define r	r23

	PROC_ENTRY(libvector_memset_unaligned)

	// Hint the return from do_head, in case we go that way.
	// There's pretty much nothing to can do to hint the branch to it.
	hbrr	do_head_br, head_complete

	MR(p, p_arg)	// leaves p, the return value, in the correct reg (r3)
	BRZ_RETURN(n)

	MODULO(t0, p, 16)	// is p%16 == 0?
	VSPLTB(c, c, 3)		// splat byte in preferred slot of c into all slots
	brnz	t0, do_head	// no, handle it
head_complete:

	/*
	 * preconditions:
	 *   p%16 == 0, n > 0
	 */
	hbrr	middle_loop_br, middle_loop

	ROUND_DOWN(an, n, 16)	// an is "aligned n"
	MODULO(n, n, 16)	// what's left over in the last quad
	brz	an, do_tail	// no whole quad words; skip to tail
	clgti	t0, an, 127	// an >= 128?
	brz	t0, middle2	// nope, go handle the cases between 0 and 112

	/*
	 * 128 bytes / iteration
	 */
	.p2align 4
middle_loop:
	ai	an, an, -128
	  stqd	c,  0*16(p)
	ai	next_p, p, 128
	  stqd	c,  1*16(p)
	cgti	cond1, an, 127
	  stqd	c,  2*16(p)

	  stqd	c,  3*16(p)
	  stqd	c,  4*16(p)
	  stqd	c,  5*16(p)
	  stqd	c,  6*16(p)

	MR(p, next_p)
	  stqd	c,  7*16-128(next_p)
	or	cond2, n, an
middle_loop_br:
	  brnz	cond1, middle_loop

	/*
	 * if an and n are both zero, return now
	 */
	BRZ_RETURN(cond2)

	/*
	 * otherwise handle last of full quad words
	 *
	 *   0 <= an < 128, p%16 == 0
	 */
middle2:
	/*
	 * if an == 0, go handle the final non-full quadword
	 */
	brz	an, do_tail
	hbrr	middle2_loop_br, middle2_loop

	.p2align 3
middle2_loop:
	ai	next_p, p, 16
	  stqd	c, 0(p)
	ai	an, an, -16
	  LMR(p, next_p)
middle2_loop_br:
	  brnz	an, middle2_loop

	/* We're done with the full quadwords. */

	/*
	 * Handle the final partial quadword.
	 * We'll be modifying only the left hand portion of the quad.
	 *
	 * preconditions:
	 *   an == 0, 0 <= n < 16, p%16 == 0
	 */
do_tail:
	HINT_RETURN(do_tail_ret)
	il	mask, -1
	sfi	t1, n, 16		// t1 = 16 - n
	lqd	old, 0(p)
	shlqby  mask, mask, t1
	selb	t0, old, c, mask
	stqd	t0, 0(p)
do_tail_ret:
	RETURN()

	/*
	 * ----------------------------------------------------------------
	 * Handle the first partial quadword
	 *
	 * preconditions:
	 *   p%16 != 0
	 *
         * postconditions:
         *   p%16 == 0 or n == 0
         *
         *        |-- m --|
         *     +----------------+----------------+
         *     |  ////////      |                |
         *     +----------------+----------------+
         *        |----- r -----|
         *        p
         * ----------------------------------------------------------------
	 */
do_head:
	lqd	old, 0(p)
	MODULO_NEG(r, p, 16)
	il	mask, -1
	UMIN(m, r, n)
	shlqby	mask, mask, m	// 1's in the top, m*8 0's in the bottom
	MR(t1, p)
	sf	t0, m, r	// t0 = r - m
	a	p, p, m		// p += m
	rotqby	mask, mask, t0	// rotate 0's to the right place
	sf	n, m, n		// n -= m
	selb	t0, c, old, mask // merge
	stqd	t0, 0(t1)
	BRZ_RETURN(n)
do_head_br:
	br	head_complete
