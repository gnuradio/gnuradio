/* -*- c++ -*- */
/*
 * Copyright 2007,2009 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gruel/pmt_pool.h>
#include <algorithm>
#include <stdint.h>

namespace pmt {

static inline size_t
ROUNDUP(size_t x, size_t stride)
{
  return ((((x) + (stride) - 1)/(stride)) * (stride));
}

pmt_pool::pmt_pool(size_t itemsize, size_t alignment,
		   size_t allocation_size, size_t max_items)
  : d_itemsize(ROUNDUP(itemsize, alignment)),
    d_alignment(alignment),
    d_allocation_size(std::max(allocation_size, 16 * itemsize)),
    d_max_items(max_items), d_n_items(0),
    d_freelist(0)
{
}

pmt_pool::~pmt_pool()
{
  for (unsigned int i = 0; i < d_allocations.size(); i++){
    delete [] d_allocations[i];
  }
}

void *
pmt_pool::malloc()
{
  scoped_lock guard(d_mutex);
  item *p;

  if (d_max_items != 0){
    while (d_n_items >= d_max_items)
      d_cond.wait(guard);
  }

  if (d_freelist){	// got something?
    p = d_freelist;
    d_freelist = p->d_next;
    d_n_items++;
    return p;
  }

  // allocate a new chunk
  char *alloc = new char[d_allocation_size + d_alignment - 1];
  d_allocations.push_back(alloc);

  // get the alignment we require
  char *start = (char *)(((uintptr_t)alloc + d_alignment-1) & -d_alignment);
  char *end = alloc + d_allocation_size + d_alignment - 1;
  size_t n = (end - start) / d_itemsize;

  // link the new items onto the free list.
  p = (item *) start;
  for (size_t i = 0; i < n; i++){
    p->d_next = d_freelist;
    d_freelist = p;
    p = (item *)((char *) p + d_itemsize);
  }

  // now return the first one
  p = d_freelist;
  d_freelist = p->d_next;
  d_n_items++;
  return p;
}

void
pmt_pool::free(void *foo)
{
  if (!foo)
    return;

  scoped_lock guard(d_mutex);

  item *p = (item *) foo;
  p->d_next = d_freelist;
  d_freelist = p;
  d_n_items--;
  if (d_max_items != 0)
    d_cond.notify_one();
}

} /* namespace pmt */
