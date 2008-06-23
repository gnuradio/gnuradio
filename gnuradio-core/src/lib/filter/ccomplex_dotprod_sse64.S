#
# Copyright 2002,2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 


# input and taps are guarenteed to be 16 byte aligned.
# n_2_ccomplex_blocks is != 0
#	
#
#  ccomplex_dotprod_generic (const float *input,
#                         const float *taps, unsigned n_2_ccomplex_blocks, float *result)
#  {
#    float sum0 = 0;
#    float sum1 = 0;
#    float sum2 = 0;
#    float sum3 = 0;
#  
#    do {
#
#      sum0 += input[0] * taps[0] - input[1] * taps[1];
#      sum1 += input[0] * taps[1] + input[1] * taps[0];
#      sum2 += input[2] * taps[2] - input[3] * taps[3];
#      sum3 += input[2] * taps[3] + input[3] * taps[2];
#  
#      input += 4;
#      taps += 4;  
#  
#    } while (--n_2_ccomplex_blocks != 0);
#  
#  
#    result[0] = sum0 + sum2;
#    result[1] = sum1 + sum3;
#  }
#

# TODO: prefetch and better scheduling

#include "assembly.h"


	.file	"ccomplex_dotprod_sse64.S"
	.version	"01.01"
.text
	.p2align 4
.globl GLOB_SYMB(ccomplex_dotprod_sse)
	DEF_FUNC_HEAD(ccomplex_dotprod_sse)
GLOB_SYMB(ccomplex_dotprod_sse):

	# intput: rdi, taps: rsi, n_2_ccomplex_blocks: rdx, result: rcx

	mov	%rdx, %rax

	xorps	%xmm6, %xmm6		# zero accumulators
	
	movaps	0(%rdi), %xmm0

	xorps	%xmm7, %xmm7		# zero accumulators

	movaps	0(%rsi), %xmm2

	shr	$1, %rax		# rax = n_2_ccomplex_blocks / 2

	jmp	.L1_test

	#
	# 4 taps / loop
	# something like ?? cycles / loop
	#
	
	.p2align 4
.Loop1:	

# complex prod: C += A * B,  w/ temp Z & Y (or B), xmmPN=$0x8000000080000000
#
#	movaps	(%rdi), %xmmA
#	movaps	(%rsi), %xmmB
#
#	movaps	%xmmA, %xmmZ
#	shufps	$0xb1, %xmmZ, %xmmZ	# swap internals
#
#	mulps	%xmmB, %xmmA
#	mulps	%xmmZ, %xmmB
#
#	# SSE replacement for: pfpnacc %xmmB, %xmmA
#	xorps	%xmmPN, %xmmA
#	movaps	%xmmA, %xmmZ
#	unpcklps %xmmB, %xmmA
#	unpckhps %xmmB, %xmmZ
#	movaps	%xmmZ, %xmmY
#	shufps	$0x44, %xmmA, %xmmZ	# b01000100
#	shufps	$0xee, %xmmY, %xmmA	# b11101110
#	addps	%xmmZ, %xmmA
#
#	addps	%xmmA, %xmmC

# A=xmm0, B=xmm2, Z=xmm4
# A'=xmm1, B'=xmm3, Z'=xmm5

	movaps	16(%rdi), %xmm1

	movaps	%xmm0, %xmm4
	mulps	%xmm2, %xmm0

	shufps	$0xb1, %xmm4, %xmm4	# swap internals
	movaps	16(%rsi), %xmm3
	movaps	%xmm1, %xmm5
	addps	%xmm0, %xmm6
	mulps	%xmm3, %xmm1
	shufps	$0xb1, %xmm5, %xmm5	# swap internals
	addps	%xmm1, %xmm6
	mulps	%xmm4, %xmm2
	movaps	32(%rdi), %xmm0
	addps	%xmm2, %xmm7
	mulps	%xmm5, %xmm3

	add	$32, %rdi

	movaps	32(%rsi), %xmm2
	addps	%xmm3, %xmm7

	add	$32, %rsi



.L1_test:
	dec	%rax
	jge	.Loop1

	# We've handled the bulk of multiplies up to here.
	# Let's sse if original n_2_ccomplex_blocks was odd.
	# If so, we've got 2 more taps to do.
	
	and	$1, %rdx
	je	.Leven
	
	# The count was odd, do 2 more taps.
	# Note that we've already got mm0/mm2 preloaded
	# from the main loop.

	movaps	%xmm0, %xmm4
	mulps	%xmm2, %xmm0
	shufps	$0xb1, %xmm4, %xmm4	# swap internals
	addps	%xmm0, %xmm6
	mulps	%xmm4, %xmm2
	addps	%xmm2, %xmm7


.Leven:
	# neg inversor
	xorps	%xmm1, %xmm1
	movl	$0x80000000, -8(%rsp)
	movss	-8(%rsp), %xmm1
	shufps	$0x11, %xmm1, %xmm1	# b00010001 # 0 -0 0 -0

	# pfpnacc
	xorps	%xmm1, %xmm6

	movaps	%xmm6, %xmm2
	unpcklps %xmm7, %xmm6
	unpckhps %xmm7, %xmm2
	movaps	%xmm2, %xmm3
	shufps	$0x44, %xmm6, %xmm2	# b01000100
	shufps	$0xee, %xmm3, %xmm6	# b11101110
	addps	%xmm2, %xmm6

					# xmm6 = r1 i2 r3 i4
	movhlps	%xmm6, %xmm4		# xmm4 = r3 i4 ?? ??
	addps	%xmm4, %xmm6		# xmm6 = r1+r3 i2+i4 ?? ??
	movlps	%xmm6, (%rcx)		# store low 2x32 bits (complex) to memory

	retq

FUNC_TAIL(ccomplex_dotprod_sse)
	.ident	"Hand coded x86_64 SSE assembly"

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
