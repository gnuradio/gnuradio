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

/*!
 * \brief Public constructor for mb_runtime.
 */
mb_runtime_sptr mb_make_runtime();

/*!
 * \brief Runtime support for m-blocks
 *
 * There should generally be only a single instance of this class.
 *
 * It should be created by the top-level initialization code,
 * and that instance should be passed into the constructor of the
 * top-level mblock.
 */
class mb_runtime : boost::noncopyable
{
private:
  mb_runtime_impl_sptr	        d_impl;		  // implementation details

  mb_runtime();

  friend mb_runtime_sptr mb_make_runtime();

public:
  ~mb_runtime();

  /*!
   * \brief Run the mblocks...
   *
   * This routine turns into the m-block scheduler, and
   * blocks until the system is shutdown.
   *
   * \returns true if the system ran successfully.
   */
  bool run();
};

#endif /* INCLUDED_MB_RUNTIME_H */
