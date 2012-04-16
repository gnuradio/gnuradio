/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <volk/volk_cpu.h>
#include <volk/volk_config_fixed.h>
#include <stdlib.h>

struct VOLK_CPU volk_cpu;

#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
    #define VOLK_CPU_x86
#endif

#if defined(VOLK_CPU_x86)

//implement get cpuid for gcc compilers using a system or local copy of cpuid.h
#if defined(__GNUC__)
    #if defined(HAVE_CPUID_H)
        #include <cpuid.h>
    #else
        #include "gcc_x86_cpuid.h"
    #endif
    #define cpuid_x86(op, r) __get_cpuid(op, (unsigned int *)r+0, (unsigned int *)r+1, (unsigned int *)r+2, (unsigned int *)r+3)

//implement get cpuid for MSVC compilers using __cpuid intrinsic
#elif defined(_MSC_VER) && defined(HAVE_INTRIN_H)
    #include <intrin.h>
    #define cpuid_x86(op, r) __cpuid(r, op)

#else
    #error "A get cpuid for volk is not available on this compiler..."
#endif

static inline unsigned int cpuid_eax(unsigned int op) {
    int regs[4];
    cpuid_x86 (op, regs);
    return regs[0];
}

static inline unsigned int cpuid_ebx(unsigned int op) {
    int regs[4];
    cpuid_x86 (op, regs);
    return regs[1];
}

static inline unsigned int cpuid_ecx(unsigned int op) {
    int regs[4];
    cpuid_x86 (op, regs);
    return regs[2];
}

static inline unsigned int cpuid_edx(unsigned int op) {
    int regs[4];
    cpuid_x86 (op, regs);
    return regs[3];
}
#endif

//neon detection is linux specific
#if defined(__arm__) && defined(__linux__)
    #include <asm/hwcap.h>
    #include <linux/auxvec.h>
    #include <stdio.h>
    #define VOLK_CPU_ARM
#endif

static int has_neon(void){
#if defined(VOLK_CPU_ARM)
    FILE *auxvec_f;
    unsigned long auxvec[2];
    unsigned int found_neon = 0;
    auxvec_f = fopen("/proc/self/auxv", "rb");
    if(!auxvec_f) return 0;

    //so auxv is basically 32b of ID and 32b of value
    //so it goes like this
    while(!found_neon && auxvec_f) {
      fread(auxvec, sizeof(unsigned long), 2, auxvec_f);
      if((auxvec[0] == AT_HWCAP) && (auxvec[1] & HWCAP_NEON))
        found_neon = 1;
    }

    fclose(auxvec_f);
    return found_neon;
#else
    return 0;
#endif
}

static int has_ppc(void){
#ifdef __PPC__
    return 1;
#else
    return 0;
#endif
}

#for $arch in $archs
static int i_can_has_$arch.name () {
########################################################################
    #if $arch.type == "x86" and $arch.no_test
#if defined(VOLK_CPU_x86)
    return 1;
#else
    return 0;
#endif
########################################################################
    #else if $arch.op == 1
#if defined(VOLK_CPU_x86)
    #set $op = hex($arch.op)
    unsigned int e$(arch.reg)x = cpuid_e$(arch.reg)x ($op);
    return ((e$(arch.reg)x >> $arch.shift) & 1) == $arch.val;
#else
    return 0;
#endif
########################################################################
    #else if $arch.op == 0x80000001
#if defined(VOLK_CPU_x86)
    #set $op = hex($arch.op)
    unsigned int extended_fct_count = cpuid_eax($op);
    if (extended_fct_count < $op)
        return $(arch.val)^1;
    unsigned int extended_features = cpuid_e$(arch.reg)x ($op);
    return ((extended_features >> $arch.shift) & 1) == $arch.val;
#else
    return 0;
#endif
########################################################################
    #else if $arch.type == "powerpc"
    return has_ppc();
########################################################################
    #else if $arch.type == "arm"
    return has_neon();
########################################################################
    #else if $arch.type == "all"
    return 1;
########################################################################
    #else ##$
    return 0;
    #end if
}

#end for

void volk_cpu_init() {
    #for $arch in $archs
    volk_cpu.has_$arch.name = &i_can_has_$arch.name;
    #end for
}

unsigned int volk_get_lvarch() {
    unsigned int retval = 0;
    volk_cpu_init();
    #for $arch in $archs
    retval += volk_cpu.has_$(arch.name)() << LV_$(arch.name.upper());
    #end for
    return retval;
}
