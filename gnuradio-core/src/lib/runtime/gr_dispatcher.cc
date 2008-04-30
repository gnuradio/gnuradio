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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_dispatcher.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>

#ifdef HAVE_SELECT
#  ifdef HAVE_SYS_SELECT_H
#    include <sys/select.h>
#  else
#    ifdef HAVE_SYS_TIME_H
#      include <sys/time.h>
#    endif
#    ifdef HAVE_SYS_TYPES_H
#      include <sys/types.h>
#    endif
#    ifdef HAVE_UNISTD_H
#      include <unistd.h>
#    endif
#  endif
#endif


static gr_dispatcher_sptr s_singleton;

gr_dispatcher_sptr
gr_make_dispatcher()
{
  return gr_dispatcher_sptr(new gr_dispatcher());
}

gr_dispatcher_sptr
gr_dispatcher_singleton()
{
  if (s_singleton)
    return s_singleton;

  s_singleton = gr_make_dispatcher();
  return s_singleton;
}

#if !defined(HAVE_SELECT)		// Stub it out

gr_dispatcher::gr_dispatcher()
{
}

gr_dispatcher::~gr_dispatcher()
{
}

bool
gr_dispatcher::add_handler(gr_select_handler_sptr handler)
{
  return true;
}

bool
gr_dispatcher::del_handler(gr_select_handler_sptr handler)
{
  return true;
}

bool
gr_dispatcher::del_handler(gr_select_handler *handler)
{
  return true;
}

void
gr_dispatcher::loop(double timeout)
{
}

#else			// defined(HAVE_SELECT)

gr_dispatcher::gr_dispatcher()
  : d_handler(FD_SETSIZE), d_max_index(-1)
{
}

gr_dispatcher::~gr_dispatcher()
{
}

bool
gr_dispatcher::add_handler(gr_select_handler_sptr handler)
{
  int fd = handler->fd();
  if (fd < 0 || fd >= FD_SETSIZE)
    return false;

  d_max_index = std::max(d_max_index, fd);
  d_handler[fd] = handler;
  return true;
}

bool
gr_dispatcher::del_handler(gr_select_handler_sptr handler)
{
  return del_handler(handler.get());
}

bool
gr_dispatcher::del_handler(gr_select_handler *handler)
{
  int fd = handler->fd();
  if (fd < 0 || fd >= FD_SETSIZE)
    return false;

  d_handler[fd].reset();

  if (fd == d_max_index){
    int i;
    for (i = fd - 1; i >= 0 && !d_handler[i]; i--)
      ;
    d_max_index = i;
  }
  return true;
}


void
gr_dispatcher::loop(double timeout)
{
  struct timeval master;
  struct timeval tmp;
  fd_set	 rd_set;
  fd_set	 wr_set;

  double secs = floor (timeout);
  master.tv_sec = (long) secs;
  master.tv_usec = (long) ((timeout - secs) * 1e6);

  while (d_max_index >= 0){
    FD_ZERO(&rd_set);
    FD_ZERO(&wr_set);

    for (int i = 0; i <= d_max_index; i++){
      if (d_handler[i] && d_handler[i]->readable())
	FD_SET(i, &rd_set);
      if (d_handler[i] && d_handler[i]->writable())
	FD_SET(i, &wr_set);
    }

    tmp = master;
    int retval = select(d_max_index+1, &rd_set, &wr_set, 0, &tmp);
    if (retval == 0)	// timed out with nothing ready
      continue;
    if (retval < 0){
      if (errno == EINTR)
	continue;
      perror ("gr_dispatcher/select");
      return;
    }

    for (int i = 0; i <= d_max_index; i++){
      if (FD_ISSET(i, &rd_set))
	if (d_handler[i])
	  d_handler[i]->handle_read();
      if (FD_ISSET(i, &wr_set))
	if (d_handler[i])
	  d_handler[i]->handle_write();
    }
  }
}

#endif
