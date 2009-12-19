/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#include <boost/thread.hpp>
#include <boost/bind.hpp> //boost::bind used when creating threads

namespace gruel {

  typedef boost::mutex                     mutex;
  typedef boost::unique_lock<boost::mutex> scoped_lock;
  typedef boost::condition_variable        condition_variable;

  /*!
   * Returns absolute time 'secs' into the future
   */
  boost::system_time abs_time(double secs);

  /*!
   * Returns a delta time of 'secs' in duration
   */
  boost::posix_time::time_duration delta_time(double secs);

} /* namespace gruel */

#endif /* INCLUDED_THREAD_H */
