#
# Copyright 2002 Free Software Foundation, Inc.
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
#  sse_float_dotprod (const float *input,
#                  const float *taps, unsigned n_4_float_blocks)
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


	.file	"3dnow_float_dotprod_really_simple.s"
	.version	"01.01"
.text
	.p2align 4
.globl sse_float_dotprod
	.type	 sse_float_dotprod,@function
sse_float_dotprod:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movl	16(%ebp), %ecx

	
	# The plan is to get it computing the correct answer, and
	# then to unroll and schedule the inner loop.

	pxor	%mm4, %mm4		# mm4 = 0 0
	shll	$1, %ecx		# count * 2

	.p2align 4
.Loop1:
	movq	(%eax), %mm0
	pfmul	(%edx), %mm0
	pfadd	%mm0, %mm4
	addl	$8, %edx
	addl	$8, %eax
	decl	%ecx
	jne	.Loop1
	
	# at this point mm4 contains partial sums

	pfacc	%mm4, %mm4
	movd	%mm4, 16(%ebp)
	femms
	flds	16(%ebp)

	popl	%ebp
	ret
.Lfe1:
	.size	 sse_float_dotprod,.Lfe1-sse_float_dotprod
	.ident	"Hand coded x86 3DNow! assembly"

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
