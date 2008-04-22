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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_GC_ALIGNED_ALLOC_H
#define INCLUDED_GC_ALIGNED_ALLOC_H

#include <boost/shared_ptr.hpp>

/*!
 * \brief Return pointer to chunk of storage of size size bytes.
 * The allocation will be aligned to an \p alignment boundary.
 *
 * \param size is the number of bytes to allocate
 * \param alignment is the minimum storage alignment in bytes; must be a power of 2.
 *
 * Throws if can't allocate memory.  The storage should be freed
 * with "free" when done.  The memory is initialized to zero.
 */
void *
gc_aligned_alloc(size_t size, size_t alignment = 128);

/*!
 * \brief Return boost::shared_ptr to chunk of storage of size size bytes.
 * The allocation will be aligned to an \p alignment boundary.
 *
 * \param size is the number of bytes to allocate
 * \param alignment is the minimum storage alignment in bytes; must be a power of 2.
 *
 * Throws if can't allocate memory.  The storage should be freed
 * with "free" when done.  The memory is initialized to zero.
 */
boost::shared_ptr<void>
gc_aligned_alloc_sptr(size_t size, size_t alignment = 128);

#endif /* INCLUDED_GC_ALIGNED_ALLOC_H */
