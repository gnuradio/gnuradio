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

#ifndef INCLUDED_MEMORY_BARRIER_H
#define INCLUDED_MEMORY_BARRIER_H

/*
 * powerpc memory barriers
 *
 * The sync instruction guarantees that all memory accesses initiated
 * by this processor have been performed (with respect to all other
 * mechanisms that access memory).  The eieio instruction is a barrier
 * providing an ordering (separately) for (a) cacheable stores and (b)
 * loads and stores to non-cacheable memory (e.g. I/O devices).
 *
 * smp_mb() prevents loads and stores being reordered across this point.
 * smp_rmb() prevents loads being reordered across this point.
 * smp_wmb() prevents stores being reordered across this point.
 *
 * We have to use the sync instructions for smp_mb(), since lwsync
 * doesn't order loads with respect to previous stores.  Lwsync is
 * fine for smp_rmb(), though.  For smp_wmb(), we use eieio since it
 * is only used to order updates to system memory.
 *
 * For details, see "PowerPC Virtual Environment Architecture, Book
 * II".  Especially Chapter 1, "Storage Model" and Chapter 3, "Storage
 * Control Instructions." (site:ibm.com)
 */

#include <ppu_intrinsics.h>

static inline void smp_mb(void)
{
  __sync();
}

static inline void smp_rmb(void)
{
  __lwsync();
}

static inline void smp_wmb(void)
{
  __eieio();
}


#endif /* INCLUDED_MEMORY_BARRIER_H */
