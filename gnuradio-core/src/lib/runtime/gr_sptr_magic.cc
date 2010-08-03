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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gr_sptr_magic.h>
#include <gr_hier_block2.h>
#include <map>
#include <stdexcept>


#include <gruel/thread.h>

namespace gnuradio {

  static gruel::mutex	s_mutex;
  typedef std::map<gr_basic_block*, gr_basic_block_sptr> sptr_map;
  static sptr_map	s_map;

  void
  detail::sptr_magic::create_and_stash_initial_sptr(gr_hier_block2 *p)
  {
    gr_basic_block_sptr sptr(p);
    gruel::scoped_lock guard(s_mutex);
    s_map.insert(sptr_map::value_type(static_cast<gr_basic_block *>(p), sptr));
  }


  gr_basic_block_sptr 
  detail::sptr_magic::fetch_initial_sptr(gr_basic_block *p)
  {
    /*
     * If p isn't a subclass of gr_hier_block2, just create the
     * shared ptr and return it.
     */
    gr_hier_block2 *hb2 = dynamic_cast<gr_hier_block2 *>(p);
    if (!hb2){
      return gr_basic_block_sptr(p);
    }

    /*
     * p is a subclass of gr_hier_block2, thus we've already created the shared pointer
     * and stashed it away.  Fish it out and return it.
     */
    gruel::scoped_lock guard(s_mutex);
    sptr_map::iterator pos = s_map.find(static_cast<gr_basic_block *>(p));
    if (pos == s_map.end())
      throw std::invalid_argument("gr_sptr_magic: invalid pointer!");

    gr_basic_block_sptr sptr = pos->second;
    s_map.erase(pos);
    return sptr;
  }
};
