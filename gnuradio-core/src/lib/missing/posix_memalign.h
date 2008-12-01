/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef _POSIX_MEMALIGN_H_
#define _POSIX_MEMALIGN_H_

#include <stdlib.h>

#ifndef HAVE_POSIX_MEMALIGN

#ifdef	__cplusplus
extern "C" {
#endif

extern int posix_memalign (void** memptr, size_t alignment, size_t size);

#ifdef	__cplusplus
};
#endif

#endif /* ! HAVE_POSIX_MEMALIGN */

#endif /* _POSIX_MEMALIGN_H_ */
