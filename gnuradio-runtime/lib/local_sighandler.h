/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_LOCAL_SIGHANDLER_H
#define INCLUDED_GR_LOCAL_SIGHANDLER_H

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <gnuradio/api.h>
#include <string>

namespace gr {

  /*!
   * \brief Get and set signal handler.
   *
   * \ingroup internal
   * Constructor installs new handler, destructor reinstalls
   * original value.
   */
  class GR_RUNTIME_API local_sighandler
  {
  private:
    int d_signum;
#ifdef HAVE_SIGACTION
    struct sigaction d_old_action;
#endif

  public:
    local_sighandler(int signum, void (*new_handler)(int));
    ~local_sighandler();

    /* throw gr_signal (signum) */
    static void throw_signal(int signum);
  };

  /*!
   * \brief Representation of signal.
   */
  class GR_RUNTIME_API signal
  {
  private:
    int d_signum;

  public:
    signal(int signum) : d_signum(signum) {}
    int signum() const { return d_signum; }
    std::string name() const;
  };

} /* namespace gr */

#endif /* INCLUDED_GR_LOCAL_SIGHANDLER_H */
