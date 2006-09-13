/* -*- c++ -*- */
/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_file_sink.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>

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

gr_file_sink::gr_file_sink(size_t itemsize, const char *filename)
  : gr_sync_block ("file_sink",
		   gr_make_io_signature(1, 1, itemsize),
		   gr_make_io_signature(0, 0, 0)),
    d_itemsize(itemsize), d_fp(0), d_new_fp(0), d_updated(false)
{
  if (!open(filename))
    throw std::runtime_error ("can't open file");
}

gr_file_sink_sptr
gr_make_file_sink (size_t itemsize, const char *filename)
{
  return gr_file_sink_sptr (new gr_file_sink (itemsize, filename));
}

gr_file_sink::~gr_file_sink ()
{
  close();
  if (d_fp){
    fclose((FILE *) d_fp);
    d_fp = 0;
  }
}

bool
gr_file_sink::open(const char *filename)
{
  omni_mutex_lock	l(d_mutex);	// hold mutex for duration of this function

  // we use the open system call to get access to the O_LARGEFILE flag.
  int fd;
  if ((fd = ::open (filename,
		    O_WRONLY|O_CREAT|O_TRUNC|OUR_O_LARGEFILE|OUR_O_BINARY, 0664)) < 0){
    perror (filename);
    return false;
  }

  if (d_new_fp){		// if we've already got a new one open, close it
    fclose((FILE *) d_new_fp);
    d_new_fp = 0;
  }

  if ((d_new_fp = fdopen (fd, "wb")) == NULL){
    perror (filename);
    ::close(fd);		// don't leak file descriptor if fdopen fails.
  }

  d_updated = true;
  return d_new_fp != 0;
}

void
gr_file_sink::close()
{
  omni_mutex_lock	l(d_mutex);	// hold mutex for duration of this function

  if (d_new_fp){
    fclose((FILE *) d_new_fp);
    d_new_fp = 0;
  }
  d_updated = true;
}

int 
gr_file_sink::work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  char *inbuf = (char *) input_items[0];
  int  nwritten = 0;

  if (d_updated){
    omni_mutex_lock	l(d_mutex);	// hold mutex for duration of this block
    if (d_fp)
      fclose((FILE *)d_fp);
    d_fp = d_new_fp;			// install new file pointer
    d_new_fp = 0;
    d_updated = false;
  }
  
  if (!d_fp)
    return noutput_items;		// drop output on the floor

  while (nwritten < noutput_items){
    int count = fwrite (inbuf, d_itemsize, noutput_items - nwritten, (FILE *) d_fp);
    if (count == 0)	// FIXME add error handling
      break;
    nwritten += count;
    inbuf += count * d_itemsize;
  }
  return nwritten;
}
