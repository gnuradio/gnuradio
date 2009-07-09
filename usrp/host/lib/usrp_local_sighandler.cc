/* -*- c++ -*- */
/*
 * Copyright 2004,2009 Free Software Foundation, Inc.
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

/*
 * This is actually the same as gr_local_signhandler, but with a different name.
 * We don't have a common library to put this in, so...
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <usrp/usrp_local_sighandler.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

usrp_local_sighandler::usrp_local_sighandler (int signum,
					      void (*new_handler)(int))
  : d_signum (signum)
{
#ifdef HAVE_SIGACTION
  struct sigaction new_action;
  memset (&new_action, 0, sizeof (new_action));

  new_action.sa_handler = new_handler;
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;

  if (sigaction (d_signum, &new_action, &d_old_action) < 0){
    perror ("sigaction (install new)");
    throw std::runtime_error ("sigaction");
  }
#endif
}

usrp_local_sighandler::~usrp_local_sighandler ()
{
#ifdef HAVE_SIGACTION
  if (sigaction (d_signum, &d_old_action, 0) < 0){
    perror ("sigaction (restore old)");
    throw std::runtime_error ("sigaction");
  }
#endif
}

void
usrp_local_sighandler::throw_signal(int signum) throw(usrp_signal)
{
  throw usrp_signal (signum);
}

/*
 * Semi-hideous way to may a signal number into a signal name
 */

#define SIGNAME(x) case x: return #x

std::string
usrp_signal::name () const
{
  char tmp[128];

  switch (signal ()){
#ifdef SIGHUP
    SIGNAME (SIGHUP);
#endif
#ifdef SIGINT
    SIGNAME (SIGINT);
#endif
#ifdef SIGQUIT
    SIGNAME (SIGQUIT);
#endif
#ifdef SIGILL
    SIGNAME (SIGILL);
#endif
#ifdef SIGTRAP
    SIGNAME (SIGTRAP);
#endif
#ifdef SIGABRT
    SIGNAME (SIGABRT);
#endif
#ifdef SIGBUS
    SIGNAME (SIGBUS);
#endif
#ifdef SIGFPE
    SIGNAME (SIGFPE);
#endif
#ifdef SIGKILL
    SIGNAME (SIGKILL);
#endif
#ifdef SIGUSR1
    SIGNAME (SIGUSR1);
#endif
#ifdef SIGSEGV
    SIGNAME (SIGSEGV);
#endif
#ifdef SIGUSR2
    SIGNAME (SIGUSR2);
#endif
#ifdef SIGPIPE
    SIGNAME (SIGPIPE);
#endif
#ifdef SIGALRM
    SIGNAME (SIGALRM);
#endif
#ifdef SIGTERM
    SIGNAME (SIGTERM);
#endif
#ifdef SIGSTKFLT
    SIGNAME (SIGSTKFLT);
#endif
#ifdef SIGCHLD
    SIGNAME (SIGCHLD);
#endif
#ifdef SIGCONT
    SIGNAME (SIGCONT);
#endif
#ifdef SIGSTOP
    SIGNAME (SIGSTOP);
#endif
#ifdef SIGTSTP
    SIGNAME (SIGTSTP);
#endif
#ifdef SIGTTIN
    SIGNAME (SIGTTIN);
#endif
#ifdef SIGTTOU
    SIGNAME (SIGTTOU);
#endif
#ifdef SIGURG
    SIGNAME (SIGURG);
#endif
#ifdef SIGXCPU
    SIGNAME (SIGXCPU);
#endif
#ifdef SIGXFSZ
    SIGNAME (SIGXFSZ);
#endif
#ifdef SIGVTALRM
    SIGNAME (SIGVTALRM);
#endif
#ifdef SIGPROF
    SIGNAME (SIGPROF);
#endif
#ifdef SIGWINCH
    SIGNAME (SIGWINCH);
#endif
#ifdef SIGIO
    SIGNAME (SIGIO);
#endif
#ifdef SIGPWR
    SIGNAME (SIGPWR);
#endif
#ifdef SIGSYS
    SIGNAME (SIGSYS);
#endif
  default:
#if defined (HAVE_SNPRINTF)
#if defined (SIGRTMIN) && defined (SIGRTMAX) 
    if (signal () >= SIGRTMIN && signal () <= SIGRTMAX){
      snprintf (tmp, sizeof (tmp), "SIGRTMIN + %d", signal ());
      return tmp;
    }
#endif
    snprintf (tmp, sizeof (tmp), "SIGNAL %d", signal ());
    return tmp;
#else
    return "Unknown signal";
#endif
  }
}
