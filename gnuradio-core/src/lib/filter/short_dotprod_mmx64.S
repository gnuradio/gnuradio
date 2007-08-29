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

# SIMD MMX dot product
# Equivalent to the following C code:
# long dotprod(signed short *a,signed short *b,int cnt)
# {
#	long sum = 0; 
#	cnt *= 4; 
#	while(cnt--)
#		sum += *a++ + *b++;
#	return sum;
# }
# a and b should also be 64-bit aligned, or speed will suffer greatly
# Copyright 1999, Phil Karn KA9Q
# May be used under the terms of the GNU public license
	
#include "assembly.h"


	.file	"short_dotprod_mmx64.S"
	.version	"01.01"
.text
	.p2align 3
.globl GLOB_SYMB(short_dotprod_mmx)
	DEF_FUNC_HEAD(short_dotprod_mmx)
GLOB_SYMB(short_dotprod_mmx):

	# a: rdi, b: rsi, cnt: rdx

	pxor %mm0,%mm0		# clear running sum (in two 32-bit halves)
	
# MMX dot product loop unrolled 4 times, crunching 16 terms per loop
	.p2align 4
.Loop1mmx:	sub $4,%rdx
	jl   .Loop1Done
	
	movq (%rdi),%mm1	# mm1 = a[3],a[2],a[1],a[0]
 	pmaddwd (%rsi),%mm1	# mm1 = b[3]*a[3]+b[2]*a[2],b[1]*a[1]+b[0]*a[0]
	paddd %mm1,%mm0
	
	movq 8(%rdi),%mm1
	pmaddwd 8(%rsi),%mm1
	paddd %mm1,%mm0

	movq 16(%rdi),%mm1
	pmaddwd 16(%rsi),%mm1
	paddd %mm1,%mm0

	movq 24(%rdi),%mm1
	add $32,%rdi	
	pmaddwd 24(%rsi),%mm1
	add $32,%rsi	
	paddd %mm1,%mm0

	jmp .Loop1mmx
.Loop1Done:
	
	add $4,%rdx	
	
# MMX dot product loop, not unrolled, crunching 4 terms per loop
# This could be redone as Duff's Device on the unrolled loop above
.Loop2:	sub $1,%rdx
	jl   .Loop2Done
	
	movq (%rdi),%mm1
	add $8,%rdi
	pmaddwd (%rsi),%mm1
	add $8,%rsi
	paddd %mm1,%mm0
	jmp .Loop2
.Loop2Done:
	
	movd %mm0,%edx		# right-hand word to edx
	punpckhdq %mm0,%mm0	# left-hand word to right side of %mm0
	movd %mm0,%eax
	addl %edx,%eax		# running sum now in %eax
	emms			# done with MMX
	
	retq

FUNC_TAIL(short_dotprod_mmx)
	.ident	"Hand coded x86_64 MMX assembly"

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
