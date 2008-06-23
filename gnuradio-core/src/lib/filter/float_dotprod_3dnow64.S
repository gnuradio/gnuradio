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


	.file	"float_dotprod_3dnow64.S"
	.version	"01.01"
.text
	.p2align 4
.globl GLOB_SYMB(float_dotprod_3dnow)
	DEF_FUNC_HEAD(float_dotprod_3dnow)
GLOB_SYMB(float_dotprod_3dnow):

	# intput: rdi, taps: rsi, n_2_ccomplex_blocks: rdx

	mov     %rdx, %rax

	# zero accumulators
	
	pxor	%mm4, %mm4		# mm4 = 0 0
	pxor	%mm5, %mm5		# mm5 = 0 0 
	pxor	%mm6, %mm6		# mm6 = 0 0 
	pxor	%mm7, %mm7		# mm7 = 0 0

	shr	$1, %rax		# rax = n_4_float_blocks / 2
	movq	0(%rsi), %mm0
	movq	8(%rsi), %mm1
	pxor	%mm2, %mm2
	pxor	%mm3, %mm3
	jmp	.L1_test

	#
	# 8 taps / loop
	# something like 6 cycles / loop
	#
	
	.p2align 4
.Loop1:	
	pfmul	0(%rdi), %mm0
	pfadd	%mm2, %mm6
	movq	16(%rsi), %mm2
	
	pfmul	8(%rdi), %mm1
	pfadd	%mm3, %mm7
	movq	24(%rsi), %mm3

	pfmul	16(%rdi), %mm2
	pfadd	%mm0, %mm4
	movq	32(%rsi), %mm0

	pfmul	24(%rdi), %mm3
	pfadd	%mm1, %mm5
	movq	40(%rsi), %mm1

	add	$32, %rdi
	add	$32, %rsi
.L1_test:
	dec	%rax
	jge	.Loop1

	# We've handled the bulk of multiplies up to here.
	# Now accumulate the final two additions and see if original
	# n_4_float_blocks was odd.  If so, we've got 4 more
	# taps to do.
	
	pfadd	%mm2, %mm6
	and	$1, %rdx
	pfadd	%mm3, %mm7
	je	.Leven
	
	# The count was odd, do 4 more taps.
	# Note that we've already got mm0 and mm1 preloaded
	# from the main loop.
	
	pfmul	0(%rdi), %mm0
	pfadd	%mm0, %mm4
	pfmul	8(%rdi), %mm1
	pfadd	%mm1, %mm5

.Leven:			
	# at this point mm4, mm5, mm6 and mm7 contain partial sums
	
	pfadd	%mm7, %mm6
	pfadd	%mm5, %mm4
	pfadd	%mm6, %mm4
	pfacc	%mm4, %mm4
	
	movd    %mm4, -8(%rsp)
	movss   -8(%rsp), %xmm0
	femms

	retq

FUNC_TAIL(float_dotprod_3dnow)
	.ident	"Hand coded x86_64 3DNow! assembly"

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
