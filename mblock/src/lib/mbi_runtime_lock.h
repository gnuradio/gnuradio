/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#ifndef INCLUDED_MBI_RUNTIME_LOCK_H
#define INCLUDED_MBI_RUNTIME_LOCK_H

#include <mb_runtime.h>
#include <mb_mblock_impl.h>
#include <boost/utility.hpp>

/*!
 * \brief acquire and release big runtime lock
 *
 * As an alternative to:
 * {
 *   rt->lock();
 *   .....
 *   rt->unlock();
 * }
 *
 * you can use a single instance of the mbi_runtime_lock class:
 *
 * {
 *   mbi_runtime_lock l(rt);
 *   ....
 * }
 *
 * This has the advantage that rt->unlock() will be called automatically
 * when an exception is thrown.
 */

class mbi_runtime_lock : boost::noncopyable {
  mb_runtime	*d_rt;
public:
  mbi_runtime_lock(mb_runtime *rt) : d_rt(rt) { d_rt->lock(); }
  mbi_runtime_lock(mb_mblock_impl *mi) : d_rt(mi->runtime()) { d_rt->lock(); }
  mbi_runtime_lock(mb_mblock *mb) : d_rt(mb->impl()->runtime()) { d_rt->lock(); }
  ~mbi_runtime_lock(void) { d_rt->unlock(); }

};

#endif /* INCLUDED_MBI_RUNTIME_LOCK_H */

