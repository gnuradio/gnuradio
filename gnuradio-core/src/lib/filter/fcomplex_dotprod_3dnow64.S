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
# n_2_complex_blocks is != 0
#	
#
#  fcomplex_dotprod_generic (const float *input,
#                         const float *taps, unsigned n_2_complex_blocks, float *result)
#  {
#    float sum0 = 0;
#    float sum1 = 0;
#    float sum2 = 0;
#    float sum3 = 0;
#  
#    do {
#  
#      sum0 += input[0] * taps[0];
#      sum1 += input[0] * taps[1];
#      sum2 += input[1] * taps[2];
#      sum3 += input[1] * taps[3];
#  
#      input += 2;
#      taps += 4;
#  
#    } while (--n_2_complex_blocks != 0);
#  
#  
#    result[0] = sum0 + sum2;
#    result[1] = sum1 + sum3;
#  }
#  		

#include "assembly.h"


	.file	"fcomplex_dotprod_3dnow64.S"
	.version	"01.01"
.text
	.p2align 4
.globl GLOB_SYMB(fcomplex_dotprod_3dnow)
	DEF_FUNC_HEAD(fcomplex_dotprod_3dnow)
GLOB_SYMB(fcomplex_dotprod_3dnow):

	# intput: rdi, taps: rsi, n_2_ccomplex_blocks: rdx, result: rcx

	mov	%rdx, %rax

	# zero accumulators
	
	pxor	%mm4, %mm4		# mm4 = 0 0
	pxor	%mm5, %mm5		# mm5 = 0 0 
	pxor	%mm6, %mm6		# mm6 = 0 0 
	pxor	%mm7, %mm7		# mm7 = 0 0

	shr	$1, %rax		# rax = n_2_complex_blocks / 2

	movq	0(%rdi), %mm0

	pxor	%mm2, %mm2
	pxor	%mm3, %mm3

	movq	%mm0, %mm1
	punpckldq	%mm0, %mm0
	punpckhdq	%mm1, %mm1


	jmp	.L1_test

	#
	# 4 taps / loop
	# something like ?? cycles / loop
	#
	
	.p2align 4
.Loop1:	
	pfmul	0(%rsi), %mm0
	pfadd	%mm2, %mm6

	movq	8(%rdi), %mm2

	pfadd	%mm3, %mm7

	pfmul	8(%rsi), %mm1

	movq	%mm2, %mm3
	punpckldq	%mm2, %mm2
	punpckhdq	%mm3, %mm3


	pfmul	16(%rsi), %mm2
	pfadd	%mm0, %mm4

	movq	16(%rdi), %mm0

	pfadd	%mm1, %mm5

	movq	%mm0, %mm1
	punpckldq	%mm0, %mm0

	pfmul	24(%rsi), %mm3

	punpckhdq	%mm1, %mm1


#TODO: add prefetch?

	add	$32, %rsi
	add	$16, %rdi

.L1_test:
	dec	%rax
	jge	.Loop1

	# We've handled the bulk of multiplies up to here.
	# Now accumulate the final two additions and see if original
	# n_2_complex_blocks was odd.  If so, we've got 2 more
	# taps to do.
	
	pfadd	%mm2, %mm6
	and	$1, %rdx
	pfadd	%mm3, %mm7
	je	.Leven
	
	# The count was odd, do 2 more taps.
	# Note that we've already got mm0 and mm1 preloaded
	# from the main loop.
	
	pfmul	0(%rsi), %mm0
	pfadd	%mm0, %mm4
	pfmul	8(%rsi), %mm1
	pfadd	%mm1, %mm5


.Leven:
	# at this point mm4, mm5, mm6 and mm7 contain partial sums
	
	pfadd	%mm7, %mm6
	pfadd	%mm5, %mm4
	pfadd	%mm6, %mm4

	movq	%mm4, (%rcx)		# result
	femms

	retq

FUNC_TAIL(fcomplex_dotprod_3dnow)
	.ident	"Hand coded x86_64 3DNow! assembly"

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
