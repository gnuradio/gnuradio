/* -*- c++ -*- */
/*
 * Copyright 2006,2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_RUNTIME_H
#define INCLUDED_MB_RUNTIME_H

#include <mblock/common.h>
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
protected:  
  mb_mblock_sptr	d_top;

public:
  mb_runtime(){}
  virtual ~mb_runtime();

  /*!
   * \brief Construct and run the specified mblock hierarchy.
   *
   * This routine turns into the m-block scheduler, and
   * blocks until the system is shutdown.
   *
   * \param name name of the top-level mblock (conventionally "top")
   * \param class_name The class of the top-level mblock to create.
   * \param user_arg The argument to pass to the top-level mblock constructor
   *
   * \returns true if the system ran successfully.
   */
  virtual bool run(const std::string &instance_name,
		   const std::string &class_name,
		   pmt_t user_arg,
		   pmt_t *result = 0) = 0;

  // QA only...
  mb_mblock_sptr top() { return d_top; }
};

#endif /* INCLUDED_MB_RUNTIME_H */
