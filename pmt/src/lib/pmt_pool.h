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
#ifndef INCLUDED_PMT_POOL_H
#define INCLUDED_PMT_POOL_H

#include <cstddef>
#include <omnithread.h>
#include <vector>

/*!
 * \brief very simple thread-safe fixed-size allocation pool
 *
 * FIXME may want to go to global allocation with per-thread free list.
 * This would eliminate virtually all lock contention.
 */
class pmt_pool {

  struct item {
    struct item	*d_next;
  };
  
  omni_mutex	      d_mutex;
  
  size_t	      d_itemsize;
  size_t	      d_alignment;
  size_t	      d_allocation_size;
  item	       	     *d_freelist;
  std::vector<char *> d_allocations;

public:
  /*!
   * \param itemsize size in bytes of the items to be allocated.
   * \param alignment alignment in bytes of all objects to be allocated (must be power-of-2).
   * \param allocation_size number of bytes to allocate at a time from the underlying allocator.
   */
  pmt_pool(size_t itemsize, size_t alignment = 16, size_t allocation_size = 4096);
  ~pmt_pool();

  void *malloc();
  void free(void *p);
};

#endif /* INCLUDED_PMT_POOL_H */
