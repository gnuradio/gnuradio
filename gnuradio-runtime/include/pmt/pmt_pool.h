/* -*- c++ -*- */
/*
 * Copyright 2007,2009,2013 Free Software Foundation, Inc.
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

#include <pmt/api.h>
#include <cstddef>
#include <vector>
#include <boost/thread.hpp>

namespace pmt {

/*!
 * \brief very simple thread-safe fixed-size allocation pool
 *
 * FIXME may want to go to global allocation with per-thread free list.
 * This would eliminate virtually all lock contention.
 */
class PMT_API pmt_pool {

  struct PMT_API item {
    struct item	*d_next;
  };

  typedef boost::unique_lock<boost::mutex>  scoped_lock;
  mutable boost::mutex 		d_mutex;
  boost::condition_variable	d_cond;

  size_t	      d_itemsize;
  size_t	      d_alignment;
  size_t	      d_allocation_size;
  size_t	      d_max_items;
  size_t	      d_n_items;
  item	       	     *d_freelist;
  std::vector<char *> d_allocations;

public:
  /*!
   * \param itemsize size in bytes of the items to be allocated.
   * \param alignment alignment in bytes of all objects to be allocated (must be power-of-2).
   * \param allocation_size number of bytes to allocate at a time from the underlying allocator.
   * \param max_items is the maximum number of items to allocate.  If this number is exceeded,
   *	      the allocate blocks.  0 implies no limit.
   */
  pmt_pool(size_t itemsize, size_t alignment = 16,
	   size_t allocation_size = 4096, size_t max_items = 0);
  ~pmt_pool();

  void *malloc();
  void free(void *p);
};

} /* namespace pmt */

#endif /* INCLUDED_PMT_POOL_H */
