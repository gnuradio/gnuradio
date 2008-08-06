/* -*- c++ -*- */
/*
 * Copyright 2002,2008 Free Software Foundation, Inc.
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

#include <gr_cpu.h>

/*
 * execute CPUID instruction, return EAX, EBX, ECX and EDX values in result
 */
extern "C" {
void cpuid_x86 (unsigned int op, unsigned int result[4]);
};

/*
 * CPUID functions returning a single datum
 */

static inline unsigned int cpuid_eax(unsigned int op)
{
  unsigned int	regs[4];
  cpuid_x86 (op, regs);
  return regs[0];
}

static inline unsigned int cpuid_ebx(unsigned int op)
{
  unsigned int	regs[4];
  cpuid_x86 (op, regs);
  return regs[1];
}

static inline unsigned int cpuid_ecx(unsigned int op)
{
  unsigned int	regs[4];
  cpuid_x86 (op, regs);
  return regs[2];
}

static inline unsigned int cpuid_edx(unsigned int op)
{
  unsigned int	regs[4];
  cpuid_x86 (op, regs);
  return regs[3];
}

// ----------------------------------------------------------------

bool
gr_cpu::has_mmx ()
{
  unsigned int edx = cpuid_edx (1);	// standard features
  return (edx & (1 << 23)) != 0;
}

bool
gr_cpu::has_sse ()
{
  unsigned int edx = cpuid_edx (1);	// standard features
  return (edx & (1 << 25)) != 0;
}

bool
gr_cpu::has_sse2 ()
{
  unsigned int edx = cpuid_edx (1);	// standard features
  return (edx & (1 << 26)) != 0;
}

bool
gr_cpu::has_sse3 ()
{
  unsigned int ecx = cpuid_ecx (1);	// standard features
  return (ecx & (1 << 0)) != 0;
}

bool
gr_cpu::has_ssse3 ()
{
  unsigned int ecx = cpuid_ecx (1);	// standard features
  return (ecx & (1 << 9)) != 0;
}

bool
gr_cpu::has_sse4_1 ()
{
  unsigned int ecx = cpuid_ecx (1);	// standard features
  return (ecx & (1 << 19)) != 0;
}

bool
gr_cpu::has_sse4_2 ()
{
  unsigned int ecx = cpuid_ecx (1);	// standard features
  return (ecx & (1 << 20)) != 0;
}


bool
gr_cpu::has_3dnow ()
{
  unsigned int extended_fct_count = cpuid_eax (0x80000000);
  if (extended_fct_count < 0x80000001)
    return false;

  unsigned int extended_features = cpuid_edx (0x80000001);
  return (extended_features & (1 << 31)) != 0;
}

bool
gr_cpu::has_3dnowext ()
{
  unsigned int extended_fct_count = cpuid_eax (0x80000000);
  if (extended_fct_count < 0x80000001)
    return false;

  unsigned int extended_features = cpuid_edx (0x80000001);
  return (extended_features & (1 << 30)) != 0;
}
