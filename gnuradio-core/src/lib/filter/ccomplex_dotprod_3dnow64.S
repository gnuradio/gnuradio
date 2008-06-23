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


	.file	"ccomplex_dotprod_3dnow64.S"
	.version	"01.01"
.text
	.p2align 4
.globl GLOB_SYMB(ccomplex_dotprod_3dnow)
	DEF_FUNC_HEAD(ccomplex_dotprod_3dnow)
GLOB_SYMB(ccomplex_dotprod_3dnow):

	# intput: rdi, taps: rsi, n_2_ccomplex_blocks: rdx, result: rcx

	mov     %rdx, %rax

	# zero accumulators
	
	pxor	%mm6, %mm6		# mm6 = 0 0 

	movq	0(%rdi), %mm0

	pxor	%mm7, %mm7		# mm7 = 0 0 

	movq	0(%rsi), %mm2

	movq	8(%rdi), %mm1

	shr	$1, %rax		# rax = n_2_ccomplex_blocks / 2

	movq	8(%rsi), %mm3

	jmp	.L1_test

	#
	# 4 taps / loop
	# something like ?? cycles / loop
	#
	
	.p2align 4
.Loop1:	

# complex prod: C += A * B,  w/ temp Z, mmPN=$80000000
#
#       movq    (%rdx), %mmA
#       movq    (%rsi), %mmB
#
#	# 3DNow! replacement for: pswapd  %mmA, %mmZ
#	# TODO: optimize the punpckhdq
#	movq	%mmA, %mmZ
#	punpckhdq	%mmZ, %mmZ
#	punpckldq	%mmA, %mmZ
#
#       pfmul   %mmB, %mmA
#       pfmul   %mmZ, %mmB
#
#	# 3DNow! replacement for: pfpnacc %mmB, %mmA
#	pxor	%mmPN, %mmA
#	pfacc	%mmB, %mmA
#
#       pfadd   %mmA, %mmC


# A=mm0, B=mm2, Z=mm4
# A'=mm1, B'=mm3, Z'=mm5

	movq	%mm0, %mm4
	movq	%mm1, %mm5
	punpckhdq	%mm4, %mm4
	punpckhdq	%mm5, %mm5
	punpckldq	%mm0, %mm4
	pfmul	%mm2, %mm0
	punpckldq	%mm1, %mm5
	pfmul	%mm4, %mm2
	pfadd	%mm0, %mm6
	movq	16(%rsi), %mm0
	pfmul	%mm3, %mm1
	pfadd	%mm2, %mm7
	movq	16(%rdi), %mm2
	pfadd	%mm1, %mm6
	pfmul	%mm5, %mm3
	movq	24(%rsi), %mm1

	movq	%mm0, %mm4
	movq	%mm1, %mm5

	pfadd	%mm3, %mm7
	movq	24(%rdi), %mm3

# unroll

	punpckhdq	%mm4, %mm4
	punpckhdq	%mm5, %mm5
	punpckldq	%mm0, %mm4
	pfmul	%mm2, %mm0
	punpckldq	%mm1, %mm5
	pfmul	%mm4, %mm2
	pfadd	%mm0, %mm6
	movq	32(%rsi), %mm0
	pfmul	%mm3, %mm1
	pfadd	%mm2, %mm7
	movq	32(%rdi), %mm2
	pfadd	%mm1, %mm6
	pfmul	%mm5, %mm3
	movq	40(%rsi), %mm1

	add	$32, %rdi
	add	$32, %rsi

	pfadd	%mm3, %mm7
	movq	8(%rdi), %mm3

.L1_test:
	dec	%rax
	jge	.Loop1

	# We've handled the bulk of multiplies up to here.
	# Let's see if original n_2_ccomplex_blocks was odd.
	# If so, we've got 2 more taps to do.
	
	and	$1, %rdx
	je	.Leven
	
	# The count was odd, do 2 more taps.
	# Note that we've already got mm0/mm2 & mm1/mm3 preloaded
	# from the main loop.

	movq	%mm0, %mm4
	movq	%mm1, %mm5
	punpckhdq	%mm4, %mm4
	punpckhdq	%mm5, %mm5
	punpckldq	%mm0, %mm4
	pfmul	%mm2, %mm0
	punpckldq	%mm1, %mm5
	pfmul	%mm4, %mm2
	pfadd	%mm0, %mm6
	pfmul	%mm3, %mm1
	pfadd	%mm2, %mm7
	pfmul	%mm5, %mm3
	pfadd	%mm1, %mm6
	pfadd	%mm3, %mm7

.Leven:
	# mmNP: negative inversor

	pcmpeqd %mm0, %mm0		# set all bits to 1
	psllq   $63, %mm0		# keep only hsb

	pxor	%mm0, %mm6
	pfacc	%mm7, %mm6

	movq	%mm6, (%rcx)

	femms

	retq

FUNC_TAIL(ccomplex_dotprod_3dnow)
	.ident	"Hand coded x86_64 3DNow! assembly"


#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
