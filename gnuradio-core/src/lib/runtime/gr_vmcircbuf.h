/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

#ifndef _GR_VMCIRCBUF_H_
#define _GR_VMCIRCBUF_H_

#include <gr_core_api.h>
#include <vector>

/*!
 * \brief abstract class to implement doubly mapped virtual memory circular buffers
 * \ingroup internal
 */
class GR_CORE_API gr_vmcircbuf {
 protected:
  int	 d_size;
  char	*d_base;

  // CREATORS
  gr_vmcircbuf (int size) : d_size (size), d_base (0) {};

 public:
  virtual ~gr_vmcircbuf ();

  // ACCESSORS
  void *pointer_to_first_copy ()  const { return d_base; }
  void *pointer_to_second_copy () const { return d_base + d_size; }
};

/*!
 * \brief abstract factory for creating circular buffers
 */
class GR_CORE_API gr_vmcircbuf_factory {
 protected:
  gr_vmcircbuf_factory () {};
  virtual ~gr_vmcircbuf_factory ();

 public:

  /*!
   * \brief return name of this factory
   */
  virtual const char *name () const = 0;

  /*!
   * \brief return granularity of mapping, typically equal to page size
   */
  virtual int granularity () = 0;

  /*!
   * \brief return a gr_vmcircbuf, or 0 if unable.
   *
   * Call this to create a doubly mapped circular buffer.
   */
  virtual gr_vmcircbuf *make (int size) = 0;
};

/*
 * \brief pulls together all implementations of gr_vmcircbuf
 */
class GR_CORE_API gr_vmcircbuf_sysconfig {
 public:

  /*
   * \brief return the single instance of the default factory.
   *
   * returns the default factory to use if it's already defined,
   * else find the first working factory and use it.
   */
  static gr_vmcircbuf_factory *get_default_factory ();

    
  static int granularity () 	       { return get_default_factory()->granularity(); }
  static gr_vmcircbuf *make (int size) { return get_default_factory()->make(size);    }
  

  // N.B. not all factories are guaranteed to work.
  // It's too hard to check everything at config time, so we check at runtime
  static std::vector<gr_vmcircbuf_factory *> all_factories ();

  // make this factory the default
  static void set_default_factory (gr_vmcircbuf_factory *f);

  /*!
   * \brief  Does this factory really work?
   *
   * verbose = 0: silent
   * verbose = 1: names of factories tested and results
   * verbose = 2: all intermediate results
   */
  static bool test_factory (gr_vmcircbuf_factory *f, int verbose);

  /*!
   * \brief Test all factories, return true if at least one of them works
   * verbose = 0: silent
   * verbose = 1: names of factories tested and results
   * verbose = 2: all intermediate results
   */
  static bool test_all_factories (int verbose);
};


#endif /* _GR_VMCIRCBUF_H_ */
