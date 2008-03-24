/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "gc_delay.h"
#include <compiler.h>

inline static void
gc_udelay_1us(void)
{
  unsigned int i = 158;

  do {  // 20 clocks per iteration of the loop
    asm ("nop $127; nop $127; nop $127; nop $127; nop $127");
    asm ("nop $127; nop $127; nop $127; nop $127; nop $127");
    asm ("nop $127; nop $127; nop $127; nop $127; nop $127");
    asm ("nop $127; nop $127");
  } while(--i != 0);
}

void
gc_udelay(unsigned int usecs)
{
  unsigned int i;
  for (i = 0; i < usecs; i++)
    gc_udelay_1us();
}

void
gc_cdelay(unsigned int cpu_cycles)
{
  if (cpu_cycles < 40)	// roughly the amount of overhead
    return;
  
  cpu_cycles >>= 2;	// about 4 cycles / loop

  while (cpu_cycles-- != 0){
    asm ("nop $127");	// keeps compiler from removing the loop
  }
}

