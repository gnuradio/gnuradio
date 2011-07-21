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
#ifndef INCLUDED_GR_SPTR_MAGIC_H
#define INCLUDED_GR_SPTR_MAGIC_H

#include <gr_core_api.h>
#include <boost/shared_ptr.hpp>

class gr_basic_block;
class gr_hier_block2;

namespace gnuradio {

  namespace detail {
    
    class GR_CORE_API sptr_magic {
    public:
      static boost::shared_ptr<gr_basic_block> fetch_initial_sptr(gr_basic_block *p);
      static void create_and_stash_initial_sptr(gr_hier_block2 *p);
    };
  };

  /* 
   * \brief New!  Improved!  Standard method to get/create the boost::shared_ptr for a block.
   */
  template<class T>
  boost::shared_ptr<T>
  get_initial_sptr(T *p)
  {
    return boost::dynamic_pointer_cast<T, gr_basic_block>(detail::sptr_magic::fetch_initial_sptr(p));
  }
};

#endif /* INCLUDED_GR_SPTR_MAGIC_H */
