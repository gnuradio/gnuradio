/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP_LOCAL_SIGHANDLER_H
#define INCLUDED_USRP_LOCAL_SIGHANDLER_H

#include <signal.h>
#include <string>

/*!
 * \brief Representation of signal.
 */
class usrp_signal
{
  int	d_signum;
public:
  usrp_signal (int signum) : d_signum (signum) {}
  int signal () const { return d_signum; }
  std::string name () const;
};


/*!
 * \brief Get and set signal handler.
 *
 * Constructor installs new handler, destructor reinstalls
 * original value.
 */
class usrp_local_sighandler {
  int			d_signum;
#ifdef HAVE_SIGACTION
  struct sigaction	d_old_action;
#endif
public:
  usrp_local_sighandler (int signum, void (*new_handler)(int));
  ~usrp_local_sighandler ();

  /* throw usrp_signal (signum) */
  static void throw_signal (int signum) throw (usrp_signal);
};

#endif /* INCLUDED_USRP_LOCAL_SIGHANDLER_H */
