/* -*- c -*- */
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

#ifndef INCLUDED_GCELL_GC_TYPES_H
#define INCLUDED_GCELL_GC_TYPES_H

#include <stdint.h>
#include <gcell/gc_cdefs.h>
#include <gcell/compiler.h>

__GC_BEGIN_DECLS

#ifndef __cplusplus
typedef int bool;
#define true  1
#define false 0
#endif

/*!
 * \brief 64-bit integer type representing an effective address (EA)
 *
 * This type is always 64-bits, regardless of whether we're
 * running in 32 or 64-bit mode.
 */
typedef uint64_t	gc_eaddr_t;

#if !defined(__SPU__)
static inline void *
ea_to_ptr(gc_eaddr_t ea)
{
  // in 32-bit mode we're tossing the top 32-bits.
  return (void *) (uintptr_t) ea;
}

static inline gc_eaddr_t
ptr_to_ea(void *p)
{
  // two steps to avoid compiler warning in 32-bit mode.
  return (gc_eaddr_t) (uintptr_t) p;
}
#endif

__GC_END_DECLS

#endif /* INCLUDED_GCELL_GC_TYPES_H */
