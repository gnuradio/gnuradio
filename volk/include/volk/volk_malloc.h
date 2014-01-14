/* -*- c -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOLK_MALLOC_H
#define INCLUDED_VOLK_MALLOC_H

#include <volk/volk_common.h>
#include <stdlib.h>

__VOLK_DECL_BEGIN

/*!
 * \brief Allocate \p size bytes of data aligned to \p alignment.
 *
 * \details
 * Because we don't have a standard method to allocate buffers in
 * memory that are guaranteed to be on an alignment, VOLK handles this
 * itself. The volk_malloc function behaves like malloc in that it
 * returns a pointer to the allocated memory. However, it also takes
 * in an alignment specfication, which is usually something like 16 or
 * 32 to ensure that the aligned memory is located on a particular
 * byte boundary for use with SIMD.
 *
 * Internally, the volk_malloc first checks if the compiler is C11
 * compliant and uses the new aligned_alloc method. If not, it checks
 * if the system is POSIX compliant and uses posix_memalign. If that
 * fails, volk_malloc handles the memory allocation and alignment
 * internally.
 *
 * Because of the ways in which volk_malloc may allocate memory, it is
 * important to always free volk_malloc pointers using volk_free.
 *
 * \param size The number of bytes to allocate.
 * \param alignment The byte alignment of the allocated memory.
 * \return pointer to aligned memory.
 */
VOLK_API void *volk_malloc(size_t size, size_t alignment);

/*!
 * \brief Free's memory allocated by volk_malloc.
 * \param aptr The aligned pointer allocaed by volk_malloc.
 */
VOLK_API void volk_free(void *aptr);

__VOLK_DECL_END

#endif /* INCLUDED_VOLK_MALLOC_H */
