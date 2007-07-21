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

#ifndef _SSE_DEBUG_H_
#define	_SSE_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

  union xmm_register {
    unsigned long	ul[4];
    float		f[4];
  };
  
  struct xmm_regs {
    union xmm_register	xmm[8];
  };

  // callable from asm, dumps all xmm regs
  void dump_xmm_regs (void);

  void get_xmm_regs (struct xmm_regs *x);

#ifdef __cplusplus
}
#endif

#endif // _SSE_DEBUG_H_
