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

#ifndef INCLUDED_GC_SPU_MACS_H
#define INCLUDED_GC_SPU_MACS_H

/*
 * This file contains a set of macros that are generally useful when
 * coding in SPU assembler
 *
 * Note that the multi-instruction macros in here may overwrite
 * registers 77, 78, and 79 without warning.
 */

/*
 * defines for all registers
 */
#define r0	$0
#define r1	$1
#define r2	$2
#define r3	$3
#define r4	$4
#define r5	$5
#define r6	$6
#define r7	$7
#define r8	$8
#define r9	$9
#define r10	$10
#define r11	$11
#define r12	$12
#define r13	$13
#define r14	$14
#define r15	$15
#define r16	$16
#define r17	$17
#define r18	$18
#define r19	$19
#define r20	$20
#define r21	$21
#define r22	$22
#define r23	$23
#define r24	$24
#define r25	$25
#define r26	$26
#define r27	$27
#define r28	$28
#define r29	$29
#define r30	$30
#define r31	$31
#define r32	$32
#define r33	$33
#define r34	$34
#define r35	$35
#define r36	$36
#define r37	$37
#define r38	$38
#define r39	$39
#define r40	$40
#define r41	$41
#define r42	$42
#define r43	$43
#define r44	$44
#define r45	$45
#define r46	$46
#define r47	$47
#define r48	$48
#define r49	$49
#define r50	$50
#define r51	$51
#define r52	$52
#define r53	$53
#define r54	$54
#define r55	$55
#define r56	$56
#define r57	$57
#define r58	$58
#define r59	$59
#define r60	$60
#define r61	$61
#define r62	$62
#define r63	$63
#define r64	$64
#define r65	$65
#define r66	$66
#define r67	$67
#define r68	$68
#define r69	$69
#define r70	$70
#define r71	$71
#define r72	$72
#define r73	$73
#define r74	$74
#define r75	$75
#define r76	$76
#define r77	$77
#define r78	$78
#define r79	$79
#define r80	$80
#define r81	$81
#define r82	$82
#define r83	$83
#define r84	$84
#define r85	$85
#define r86	$86
#define r87	$87
#define r88	$88
#define r89	$89
#define r90	$90
#define r91	$91
#define r92	$92
#define r93	$93
#define r94	$94
#define r95	$95
#define r96	$96
#define r97	$97
#define r98	$98
#define r99	$99
#define r100	$100
#define r101	$101
#define r102	$102
#define r103	$103
#define r104	$104
#define r105	$105
#define r106	$106
#define r107	$107
#define r108	$108
#define r109	$109
#define r110	$110
#define r111	$111
#define r112	$112
#define r113	$113
#define r114	$114
#define r115	$115
#define r116	$116
#define r117	$117
#define r118	$118
#define r119	$119
#define r120	$120
#define r121	$121
#define r122	$122
#define r123	$123
#define r124	$124
#define r125	$125
#define r126	$126
#define r127	$127


#define	lr	r0	// link register
#define	sp	r1	// stack pointer
                        // r2 is environment pointer for langs that need it (ALGOL)

#define	retval	r3	// return values are passed in regs starting at r3

#define	arg1	r3	// args are passed in regs starting at r3
#define	arg2	r4
#define	arg3	r5
#define	arg4	r6
#define	arg5	r7
#define	arg6	r8
#define	arg7	r9
#define	arg8	r10
#define	arg9	r11
#define	arg10	r12

//  r3 -  r74 are volatile (caller saves)
// r74 -  r79 are volatile (scratch regs possibly destroyed by fct prolog/epilog)
// r80 - r127 are non-volatile (caller-saves)

// scratch registers reserved for use by the macros in this file.

#define _gc_t0	r79
#define	_gc_t1	r78
#define	_gc_t2	r77

/*
 * ----------------------------------------------------------------
 * 		    	    pseudo ops
 * ----------------------------------------------------------------
 */
#define PROC_ENTRY(name)		\
        .text;				\
	.p2align 4;			\
	.global	name;			\
	.type	name, @function;	\
name:

/*
 * ----------------------------------------------------------------
 * 		    aliases for common operations
 * ----------------------------------------------------------------
 */

// Move register (even pipe, 2 cycles)
#define MR(rt, ra) 			or	rt, ra, ra;

// Move register (odd pipe, 4 cycles)
#define	LMR(rt, ra) 			rotqbyi	rt, ra, 0;

// return
#define	RETURN() 			bi	lr;

// hint for a return
#define	HINT_RETURN(ret_label)		hbr	ret_label, lr;

// return if zero
#define BRZ_RETURN(rt)			biz	rt, lr;

// return if not zero
#define BRNZ_RETURN(rt)			binz	rt, lr;

// return if halfword zero
#define	BRHZ_RETURN(rt)			bihz	rt, lr;

// return if halfword not zero
#define BRHNZ_RETURN(rt)		bihnz	rt, lr;


/*
 * ----------------------------------------------------------------
 * modulo like things for constant moduli that are powers of 2
 * ----------------------------------------------------------------
 */

// rt = ra & (pow2 - 1)
#define MODULO(rt, ra, pow2) \
	andi	rt, ra, (pow2)-1;

// rt = pow2 - (ra & (pow2 - 1))
#define MODULO_NEG(rt, ra, pow2) \
	andi	rt, ra, (pow2)-1;	      	\
	sfi	rt, rt, (pow2);

// rt = ra & -(pow2)
#define	ROUND_DOWN(rt, ra, pow2) \
	andi	rt, ra, -(pow2);

// rt = (ra + (pow2 - 1)) & -(pow2)
#define ROUND_UP(rt, ra, pow2) \
	ai	rt, ra, (pow2)-1;	      	\
	andi	rt, rt, -(pow2);

/*
 * ----------------------------------------------------------------
 * Splat - replicate a particular slot into all slots
 * Altivec analogs...
 * ----------------------------------------------------------------
 */

// replicate byte from slot s [0,15]
#define VSPLTB(rt, ra, s) \
	ilh	_gc_t0, (s)*0x0101;	        \
	shufb	rt, ra, ra, _gc_t0;

// replicate halfword from slot s [0,7]
#define	VSPLTH(rt, ra, s) \
	ilh	_gc_t0, 2*(s)*0x0101 + 0x0001; 	\
	shufb	rt, ra, ra, _gc_t0;

// replicate word from slot s [0,3]
#define VSPLTW(rt, ra, s) \
	iluh	_gc_t0, 4*(s)*0x0101 + 0x0001;	\
	iohl	_gc_t0, 4*(s)*0x0101 + 0x0203;	\
	shufb	rt, ra, ra, _gc_t0;

// replicate double from slot s [0,1]
#define	VSPLTD(rt, ra, s) \
	/* sp is always 16-byte aligned */ \
	cdd	_gc_t0, 8(sp);		/* 0x10111213 14151617 00010203 04050607 */ \
	rotqbyi	rt, ra, ra, (s) << 3;	/* rotate double into preferred slot 	 */ \
	shufb	rt, rt, rt, _gc_t0;

/*
 * ----------------------------------------------------------------
 * lots of min/max variations...
 *
 * On a slot by slot basis, compute the min or max
 *
 * U - unsigned, else signed
 * B,H,{} - byte, halfword, word
 * F float
 * ----------------------------------------------------------------
 */

#define MIN_SELB(rt, ra, rb, rc)	selb	rt, ra, rb, rc;
#define MAX_SELB(rt, ra, rb, rc)	selb	rt, rb, ra, rc;

	// words

#define MIN(rt, ra, rb) \
	cgt	_gc_t0, ra, rb; \
	MIN_SELB(rt, ra, rb, _gc_t0)

#define	MAX(rt, ra, rb) \
	cgt	_gc_t0, ra, rb; \
	MAX_SELB(rt, ra, rb, _gc_t0)

#define UMIN(rt, ra, rb) \
	clgt	_gc_t0, ra, rb; \
	MIN_SELB(rt, ra, rb, _gc_t0)

#define	UMAX(rt, ra, rb) \
	clgt	_gc_t0, ra, rb; \
	MAX_SELB(rt, ra, rb, _gc_t0)

	// bytes

#define MINB(rt, ra, rb) \
	cgtb	_gc_t0, ra, rb; \
	MIN_SELB(rt, ra, rb, _gc_t0)

#define	MAXB(rt, ra, rb) \
	cgtb	_gc_t0, ra, rb; \
	MAX_SELB(rt, ra, rb, _gc_t0)

#define UMINB(rt, ra, rb) \
	clgtb	_gc_t0, ra, rb; \
	MIN_SELB(rt, ra, rb, _gc_t0)

#define	UMAXB(rt, ra, rb) \
	clgtb	_gc_t0, ra, rb; \
	MAX_SELB(rt, ra, rb, _gc_t0)

	// halfwords

#define MINH(rt, ra, rb) \
	cgth	_gc_t0, ra, rb; \
	MIN_SELB(rt, ra, rb, _gc_t0)

#define	MAXH(rt, ra, rb) \
	cgth	_gc_t0, ra, rb; \
	MAX_SELB(rt, ra, rb, _gc_t0)

#define UMINH(rt, ra, rb) \
	clgth	_gc_t0, ra, rb; \
	MIN_SELB(rt, ra, rb, _gc_t0)

#define	UMAXH(rt, ra, rb) \
	clgth	_gc_t0, ra, rb; \
	MAX_SELB(rt, ra, rb, _gc_t0)

	// floats

#define FMIN(rt, ra, rb) \
	fcgt	_gc_t0, ra, rb; \
	MIN_SELB(rt, ra, rb, _gc_t0)

#define	FMAX(rt, ra, rb) \
	fcgt	_gc_t0, ra, rb; \
	MAX_SELB(rt, ra, rb, _gc_t0)

// Ignoring the sign, select the values with the minimum magnitude
#define FMINMAG(rt, ra, rb) \
	fcmgt	_gc_t0, ra, rb; \
	MIN_SELB(rt, ra, rb, _gc_t0)

// Ignoring the sign, select the values with the maximum magnitude
#define	FMAXMAG(rt, ra, rb) \
	fcmgt	_gc_t0, ra, rb; \
	MAX_SELB(rt, ra, rb, _gc_t0)


#endif /* INCLUDED_GC_SPU_MACS_H */
