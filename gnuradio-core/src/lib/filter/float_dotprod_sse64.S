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
# n_4_float_blocks is != 0
#	
#
#  float 
#  float_dotprod_generic (const float *input,
#                         const float *taps, unsigned n_4_float_blocks)
#  {
#    float sum0 = 0;
#    float sum1 = 0;
#    float sum2 = 0;
#    float sum3 = 0;
#  
#    do {
#  
#      sum0 += input[0] * taps[0];
#      sum1 += input[1] * taps[1];
#      sum2 += input[2] * taps[2];
#      sum3 += input[3] * taps[3];
#  
#      input += 4;
#      taps += 4;
#  
#    } while (--n_4_float_blocks != 0);
#  
#  
#    return sum0 + sum1 + sum2 + sum3;
#  }
#  		

#include "assembly.h"


	.file	"float_dotprod_sse64.S"
	.version	"01.01"
.text
	.p2align 4
.globl GLOB_SYMB(float_dotprod_sse)
	DEF_FUNC_HEAD(float_dotprod_sse)
GLOB_SYMB(float_dotprod_sse):

	# intput: rdi, taps: rsi, n_2_ccomplex_blocks: rdx

	mov	%rdx, %rax

	# xmm0 xmm1 xmm2 xmm3 are used to hold taps and the result of mults
	# xmm4 xmm5 xmm6 xmm7 are used to hold the accumulated results

	xorps	%xmm4, %xmm4		# zero two accumulators
	xorps	%xmm5, %xmm5		# xmm5 holds zero for use below

	# first handle any non-zero remainder of (n_4_float_blocks % 4)

	and	$0x3, %rax
	jmp	.L1_test

	.p2align 4
.Loop1:	
	movaps	(%rsi), %xmm0
	mulps	(%rdi), %xmm0
	add	$0x10, %rdi
	add	$0x10, %rsi
	addps	%xmm0, %xmm4
.L1_test:	
	dec	%rax
	jge	.Loop1

	
	# set up for primary loop which is unrolled 4 times
	
	movaps	%xmm5, %xmm6		# zero remaining accumulators
	movaps	%xmm5, %xmm7 

	shr	$2, %rdx		# n_4_float_blocks / 4
	je	.Lcleanup		# if zero, take short path

	# finish setup and loop priming

	movaps	0x00(%rsi), %xmm0
	movaps	%xmm5, %xmm2		
	movaps	0x10(%rsi), %xmm1
	movaps	%xmm5, %xmm3

	# we know rdx is not zero, we checked above,
	# hence enter loop at top

	.p2align 4
.Loop2:
	mulps	(%rdi), %xmm0
	addps	%xmm2, %xmm6
	movaps	0x20(%rsi), %xmm2

	mulps	0x10(%rdi), %xmm1
	addps	%xmm3, %xmm7
	movaps	0x30(%rsi), %xmm3

	mulps	0x20(%rdi), %xmm2
	addps	%xmm0, %xmm4
	movaps	0x40(%rsi), %xmm0

	mulps	0x30(%rdi), %xmm3
	addps	%xmm1, %xmm5
	movaps	0x50(%rsi), %xmm1

	add	$0x40, %rdi
	add	$0x40, %rsi
	dec	%rdx
	jne	.Loop2

	# OK, now we've done with all the multiplies, but
	# we still need to handle the unaccumulated
	# products in xmm2 and xmm3

	addps	%xmm2, %xmm6
	addps	%xmm3, %xmm7

	# now we want to add all accumulators into xmm4

	addps	%xmm5, %xmm4
	addps	%xmm6, %xmm7
	addps	%xmm7, %xmm4

	
	# At this point, xmm4 contains 4 partial sums.  We need
	# to compute a "horizontal add" across xmm4.  
	# This is a fairly nasty operation...
	
.Lcleanup:				# xmm4 = d1 d2 d3 d4
	xorps	%xmm0, %xmm0		# xmm0 = 0 0 0 0  (may be unnecessary)
	movhlps	%xmm4, %xmm0		# xmm0 = 0 0 d1 d2
	addps	%xmm4, %xmm0		# xmm0 = d1 d2 d1+d3 d2+d4
	movaps	%xmm0, %xmm1		# xmm1 = d1 d2 d1+d3 d2+d4
	shufps	$0xE1, %xmm4, %xmm1	# xmm1 = d1 d2 d2+d4 d1+d3
	addss	%xmm1, %xmm0		# xmm1 = d1 d2 d1+d3 d1+d2+d3+d4

	retq

FUNC_TAIL(float_dotprod_sse)
	.ident	"Hand coded x86_64 SSE assembly"

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
