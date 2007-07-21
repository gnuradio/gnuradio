/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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
#include <stdio.h>
#include <sse_debug.h>
#include <string.h>

void
format_xmm_regs (FILE *f, struct xmm_regs *r)
{
  int	i;

  for (i = 0; i < 8; i++){
    union xmm_register *x = &r->xmm[i];
    fprintf (f, "xmm%d: %08lx %08lx %08lx %08lx", i,
	     x->ul[0], x->ul[1], x->ul[2], x->ul[3]);
    fprintf (f, "  %12g %12g %12g %12g\n",
	     x->f[0],  x->f[1],  x->f[2],  x->f[3]);
  }
}


void
get_xmm_regs (struct xmm_regs *x)
{
  asm ("movups %%xmm0,0x00(%0);	\n"
       "movups %%xmm1,0x10(%0);	\n"
       "movups %%xmm2,0x20(%0);	\n"
       "movups %%xmm3,0x30(%0);	\n"
       "movups %%xmm4,0x40(%0);	\n"
       "movups %%xmm5,0x50(%0);	\n"
       "movups %%xmm6,0x60(%0);	\n"
       "movups %%xmm7,0x70(%0);	\n" : : "r" (x));
}

void 
dump_xmm_regs (void)
{
  struct xmm_regs	r;

  get_xmm_regs (&r);
  format_xmm_regs (stderr, &r);
}

