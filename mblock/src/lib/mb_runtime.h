/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_RUNTIME_H
#define INCLUDED_MB_RUNTIME_H

#include <mb_common.h>
#include <omnithread.h>

/*!
 * \brief Public constructor (factory) for mb_runtime objects.
 */
mb_runtime_sptr mb_make_runtime();

/*!
 * \brief Abstract runtime support for m-blocks
 *
 * There should generally be only a single instance of this class.
 */
class mb_runtime : boost::noncopyable,
		   public boost::enable_shared_from_this<mb_runtime>
{
  omni_mutex	d_brl;		// big runtime lock (avoid using this if possible...)

public:
  mb_runtime(){}
  virtual ~mb_runtime();

  /*!
   * \brief Run the mblock hierarchy rooted at \p top
   *
   * This routine turns into the m-block scheduler, and
   * blocks until the system is shutdown.
   *
   * \param top top-level mblock
   * \returns true if the system ran successfully.
   */
  virtual bool run(mb_mblock_sptr top) = 0;


  // ----------------------------------------------------------------
  // Stuff from here down is really private to the implementation...
  // ----------------------------------------------------------------

  /*!
   * \brief lock the big runtime lock
   * \implementation
   */
  inline void lock() { d_brl.lock(); }

  /*!
   * \brief unlock the big runtime lock
   * \implementation
   */
  inline void unlock() { d_brl.unlock(); }

};

#endif /* INCLUDED_MB_RUNTIME_H */
