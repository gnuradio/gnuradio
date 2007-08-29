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


	.file	"short_dotprod_mmx.S"
	.version	"01.01"
.text
	.p2align 3
.globl GLOB_SYMB(short_dotprod_mmx)
	DEF_FUNC_HEAD(short_dotprod_mmx)
GLOB_SYMB(short_dotprod_mmx):
	pushl %ebp
	movl %esp,%ebp
	pushl %esi
	pushl %edi
	pushl %ecx
	pushl %ebx
	movl 8(%ebp),%esi	# a
	movl 12(%ebp),%edi	# b
	movl 16(%ebp),%ecx	# cnt
	pxor %mm0,%mm0		# clear running sum (in two 32-bit halves)
	
# MMX dot product loop unrolled 4 times, crunching 16 terms per loop
	.p2align 4
.Loop1mmx:	subl $4,%ecx
	jl   .Loop1Done
	
	movq (%esi),%mm1	# mm1 = a[3],a[2],a[1],a[0]
 	pmaddwd (%edi),%mm1	# mm1 = b[3]*a[3]+b[2]*a[2],b[1]*a[1]+b[0]*a[0]
	paddd %mm1,%mm0
	
	movq 8(%esi),%mm1
	pmaddwd 8(%edi),%mm1
	paddd %mm1,%mm0

	movq 16(%esi),%mm1
	pmaddwd 16(%edi),%mm1
	paddd %mm1,%mm0

	movq 24(%esi),%mm1
	addl $32,%esi	
	pmaddwd 24(%edi),%mm1
	addl $32,%edi	
	paddd %mm1,%mm0

	jmp .Loop1mmx
.Loop1Done:
	
	addl $4,%ecx	
	
# MMX dot product loop, not unrolled, crunching 4 terms per loop
# This could be redone as Duff's Device on the unrolled loop above
.Loop2:	subl $1,%ecx
	jl   .Loop2Done
	
	movq (%esi),%mm1
	addl $8,%esi
	pmaddwd (%edi),%mm1
	addl $8,%edi
	paddd %mm1,%mm0
	jmp .Loop2
.Loop2Done:
	
	movd %mm0,%ebx		# right-hand word to ebx
	punpckhdq %mm0,%mm0	# left-hand word to right side of %mm0
	movd %mm0,%eax
	addl %ebx,%eax		# running sum now in %eax
	emms			# done with MMX
	
	popl %ebx
	popl %ecx
	popl %edi
	popl %esi
	movl %ebp,%esp
	popl	%ebp
	ret

FUNC_TAIL(short_dotprod_mmx)
	.ident	"Hand coded x86 MMX assembly"

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
