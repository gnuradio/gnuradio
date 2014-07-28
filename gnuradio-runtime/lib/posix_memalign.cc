/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "posix_memalign.h"

#ifdef HAVE_MALLOC_H
// for Cygwin valloc () prototype
#include <malloc.h>
#endif

#ifndef HAVE_POSIX_MEMALIGN

/* emulate posix_memalign functionality, to some degree */

#include <errno.h>
#include "pagesize.h"

int posix_memalign
(void **memptr, size_t alignment, size_t size)
{
  /* emulate posix_memalign functionality, to some degree */

  /* make sure the return handle is valid; return "bad address" if not valid */
  if (memptr == 0)
    return (EFAULT);
  *memptr = (void*) 0;

  /* make sure 'alignment' is a power of 2
   * and multiple of sizeof (void*)
   */

  /* make sure 'alignment' is a multiple of sizeof (void*) */
  if ((alignment % sizeof (void*)) != 0)
    return (EINVAL);

  /* make sure 'alignment' is a power of 2 */
  if ((alignment & (alignment - 1)) != 0)
    return (EINVAL);

  /* good alignment */

#if (ALIGNED_MALLOC != 0)

  /* if 'malloc' is known to be aligned, and the desired 'alignment'
   * matches is <= that provided by 'malloc', then use 'malloc'.  This
   * works on, e.g., Darwin for which malloc is 16-byte aligned.
   */
  size_t am = (size_t) ALIGNED_MALLOC;
  if (alignment <= am) {
    /* make sure ALIGNED_MALLOC is a power of 2, to guarantee that the
     * alignment is correct (since 'alignment' must be a power of 2).
     */
    if ((am & (am - 1)) != 0)
      return (EINVAL);
    /* good malloc alignment */
    *memptr = malloc (size);
  }

#endif /* (ALIGNED_MALLOC != 0) */
#ifdef HAVE_VALLOC

  if (*memptr == (void*) 0) {
    /* try valloc if it exists */
    /* cheap and easy way to make sure alignment is met, so long as it
     * is <= pagesize () */
    if (alignment <= (size_t) gr::pagesize ()) {
      *memptr = valloc (size);
    }
  }

#endif /* HAVE_VALLOC */

#if (ALIGNED_MALLOC == 0) && !defined (HAVE_VALLOC)
  /* no posix_memalign, valloc, and malloc isn't known to be aligned
   * (enough for the input arguments); no idea what to do.
   */

#error gnuradio-runtime/lib/posix_memalign.cc: Cannot find a way to alloc aligned memory.

#endif

  /* if the pointer wasn't allocated properly, return that there was
   * not enough memory to allocate; otherwise, return OK (0).
   */
  if (*memptr == (void*) 0)
    return (ENOMEM);
  else
    return (0);
};

#endif /* ! HAVE_POSIX_MEMALIGN */
