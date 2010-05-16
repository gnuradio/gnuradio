/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2007,2009 Free Software Foundation, Inc.
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

#include <gr_file_sink_base.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>
#include <gruel/thread.h>

// win32 (mingw/msvc) specific
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef O_BINARY
#define	OUR_O_BINARY O_BINARY
#else
#define	OUR_O_BINARY 0
#endif

// should be handled via configure
#ifdef O_LARGEFILE
#define	OUR_O_LARGEFILE	O_LARGEFILE
#else
#define	OUR_O_LARGEFILE 0
#endif

gr_file_sink_base::gr_file_sink_base(const char *filename, bool is_binary)
  : d_fp(0), d_new_fp(0), d_updated(false), d_is_binary(is_binary)
{
  if (!open(filename))
    throw std::runtime_error ("can't open file");
}

gr_file_sink_base::~gr_file_sink_base ()
{
  close();
  if (d_fp){
    fclose(d_fp);
    d_fp = 0;
  }
}

bool
gr_file_sink_base::open(const char *filename)
{
  gruel::scoped_lock guard(d_mutex);	// hold mutex for duration of this function

  // we use the open system call to get access to the O_LARGEFILE flag.
  int fd;
  if ((fd = ::open (filename,
		    O_WRONLY|O_CREAT|O_TRUNC|OUR_O_LARGEFILE|OUR_O_BINARY,
		    0664)) < 0){
    perror (filename);
    return false;
  }

  if (d_new_fp){		// if we've already got a new one open, close it
    fclose(d_new_fp);
    d_new_fp = 0;
  }

  if ((d_new_fp = fdopen (fd, d_is_binary ? "wb" : "w")) == NULL){
    perror (filename);
    ::close(fd);		// don't leak file descriptor if fdopen fails.
  }

  d_updated = true;
  return d_new_fp != 0;
}

void
gr_file_sink_base::close()
{
  gruel::scoped_lock guard(d_mutex);	// hold mutex for duration of this function

  if (d_new_fp){
    fclose(d_new_fp);
    d_new_fp = 0;
  }
  d_updated = true;
}

void
gr_file_sink_base::do_update()
{
  if (d_updated){
    gruel::scoped_lock guard(d_mutex);	// hold mutex for duration of this block
    if (d_fp)
      fclose(d_fp);
    d_fp = d_new_fp;			// install new file pointer
    d_new_fp = 0;
    d_updated = false;
  }
}  

void
gr_file_sink_base::set_unbuffered(bool unbuffered)
{
	d_unbuffered = unbuffered;
}
