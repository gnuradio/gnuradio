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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gcell/gc_aligned_alloc.h>
#include <stdlib.h>
#include <stdexcept>
#include <string.h>

// custom deleter of anything that can be freed with "free"
class free_deleter {
public:
  void operator()(void *p) {
    free(p);
  }
};

void *
gc_aligned_alloc(size_t size, size_t alignment)
{
  void *p = 0;
  if (posix_memalign(&p, alignment, size) != 0){
    perror("posix_memalign");
    throw std::runtime_error("memory");
  }
  memset(p, 0, size);		// zero the memory
  return p;
}

boost::shared_ptr<void>
gc_aligned_alloc_sptr(size_t size, size_t alignment)
{
  return boost::shared_ptr<void>(gc_aligned_alloc(size, alignment),
				 free_deleter());
}
