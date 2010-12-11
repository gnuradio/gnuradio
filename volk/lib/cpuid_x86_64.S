#	
# Copyright 2003,2005 Free Software Foundation, Inc.
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

#
# execute CPUID instruction, return EAX, EBX, ECX and EDX values in result
#
#  void cpuid_x86 (unsigned int op, unsigned int result[4]);
#

#include "assembly.h"

.file "cpuid_x86_64.S"
	.version	"01.01"
.text
.globl	GLOB_SYMB(cpuid_x86)
	DEF_FUNC_HEAD(cpuid_x86)
GLOB_SYMB(cpuid_x86):
	mov	%rbx, %r11	# must save in PIC mode, holds GOT pointer
	
	mov	%rdi, %rax	# op
	cpuid
	movl	%eax, 0(%rsi)	# result
	movl	%ebx, 4(%rsi)
	movl	%ecx, 8(%rsi)
	movl	%edx, 12(%rsi)
	
	mov	%r11, %rbx
	retq

FUNC_TAIL(cpuid_x86)
	.ident	"Hand coded cpuid64 assembly"
	

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
