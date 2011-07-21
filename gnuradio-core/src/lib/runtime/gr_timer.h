/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_TIMER_H
#define INCLUDED_GR_TIMER_H

#include <gr_core_api.h>
#include <gr_types.h>

class gr_timer;

typedef boost::shared_ptr<gr_timer> gr_timer_sptr;

GR_CORE_API typedef void (*gr_timer_hook)(gr_timer *, void *);

/*!
 * \brief create a timeout.
 *
 * \ingroup misc
 * gr_timer_hook is called when timer fires.
 */
GR_CORE_API gr_timer_sptr gr_make_timer (gr_timer_hook, void *);

/*!
 * \brief implement timeouts
 */
class GR_CORE_API gr_timer {
  double	d_expiry;
  double	d_period;
  gr_timer_hook	d_hook;
  void	       *d_hook_arg;

  friend GR_CORE_API gr_timer_sptr gr_make_timer (gr_timer_hook, void *);

  gr_timer (...);

public:
  ~gr_timer ();

  //! return absolute current time (seconds since the epoc).
  static double now ();
  
  /*!
   * \brief schedule timer to fire at abs_when
   * \param abs_when	absolute time in seconds since the epoc.
   */
  void schedule_at (double abs_when);

  /*!
   * \brief schedule timer to fire rel_when seconds from now.
   * \param rel_when	relative time in seconds from now.
   */
  void schedule_after (double rel_when);	// relative time in seconds

  /*!
   * \brief schedule a periodic timeout.
   * \param abs_when	absolute time to fire first time
   * \param period	time between firings
   */
  void schedule_periodic (double abs_when, double period);

  //! cancel timer
  void unschedule ();
};

#endif /* INCLUDED_GR_TIMER_H */
