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

    /* Return Intel AVX extended CPU capabilities register.
     * This function will bomb on non-AVX-capable machines, so
     * check for AVX capability before executing.
     */
    #if __GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 4
    static inline unsigned long long _xgetbv(unsigned int index){
        unsigned int eax, edx;
        __asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
        return ((unsigned long long)edx << 32) | eax;
    }
    #define __xgetbv() _xgetbv(0)
    #else
    #define __xgetbv() 0
    #endif

//implement get cpuid for MSVC compilers using __cpuid intrinsic
#elif defined(_MSC_VER) && defined(HAVE_INTRIN_H)
    #include <intrin.h>
    #define cpuid_x86(op, r) __cpuid(((int*)r), op)

    #if defined(_XCR_XFEATURE_ENABLED_MASK)
    #define __xgetbv() _xgetbv(_XCR_XFEATURE_ENABLED_MASK)
    #else
    #define __xgetbv() 0
    #endif

#else
    #error "A get cpuid for volk is not available on this compiler..."
#endif //defined(__GNUC__)

#endif //defined(VOLK_CPU_x86)

static inline unsigned int cpuid_x86_bit(unsigned int reg, unsigned int op, unsigned int bit) {
#if defined(VOLK_CPU_x86)
    unsigned int regs[4];
    cpuid_x86(op, regs);
    return regs[reg] >> bit & 0x01;
#else
    return 0;
#endif
}

static inline unsigned int check_extended_cpuid(unsigned int val) {
#if defined(VOLK_CPU_x86)
    unsigned int regs[4];
    cpuid_x86(0x80000000, regs);
    return regs[0] >= val;
#else
    return 0;
#endif
}

static inline unsigned int get_avx_enabled(void) {
#if defined(VOLK_CPU_x86)
    return __xgetbv() & 0x6;
#else
    return 0;
#endif
}

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
static int i_can_has_$arch.name (void) {
    #for $check, $params in $arch.checks
    if ($(check)($(', '.join($params))) == 0) return 0;
    #end for
    return 1;
}

#end for

#if defined(HAVE_FENV_H)
    #include <fenv.h>
    static inline void set_float_rounding(void){
        fesetround(FE_TONEAREST);
    }
#elif defined(_MSC_VER)
    #include <float.h>
    static inline void set_float_rounding(void){
        unsigned int cwrd;
        _controlfp_s(&cwrd, 0, 0);
        _controlfp_s(&cwrd, _RC_NEAR, _MCW_RC);
    }
#else
    static inline void set_float_rounding(void){
        //do nothing
    }
#endif

void volk_cpu_init() {
    #for $arch in $archs
    volk_cpu.has_$arch.name = &i_can_has_$arch.name;
    #end for
    set_float_rounding();
}

unsigned int volk_get_lvarch() {
    unsigned int retval = 0;
    volk_cpu_init();
    #for $arch in $archs
    retval += volk_cpu.has_$(arch.name)() << LV_$(arch.name.upper());
    #end for
    return retval;
}
