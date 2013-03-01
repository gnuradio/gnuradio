/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gruel/thread.h>
#include "file_source_impl.h"
#include <gr_io_signature.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>

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

namespace gr {
  namespace blocks {

    file_source::sptr file_source::make(size_t itemsize, const char *filename, bool repeat)
    {
      return gnuradio::get_initial_sptr
	(new file_source_impl(itemsize, filename, repeat));
    }

    file_source_impl::file_source_impl(size_t itemsize, const char *filename, bool repeat)
      : gr_sync_block("file_source",
		      gr_make_io_signature(0, 0, 0),
		      gr_make_io_signature(1, 1, itemsize)),
	d_itemsize(itemsize), d_fp(0), d_new_fp(0), d_repeat(repeat),
	d_updated(false)
    {
      open(filename, repeat);
    }

    file_source_impl::~file_source_impl()
    {
      fclose ((FILE*)d_fp);
    }
    
    bool
    file_source_impl::seek(long seek_point, int whence)
    {
      return fseek((FILE*)d_fp, seek_point *d_itemsize, whence) == 0;
    }


    void
    file_source_impl::open(const char *filename, bool repeat)
    {
      // obtain exclusive access for duration of this function
      gruel::scoped_lock lock(fp_mutex);

      int fd;

      // we use "open" to use to the O_LARGEFILE flag
      if((fd = ::open(filename, O_RDONLY | OUR_O_LARGEFILE | OUR_O_BINARY)) < 0) {
	perror(filename);
	throw std::runtime_error("can't open file");
      }

      if(d_new_fp) {
	fclose(d_new_fp);
	d_new_fp = 0;
      }

      if((d_new_fp = fdopen (fd, "rb")) == NULL) {
	perror(filename);
	::close(fd);	// don't leak file descriptor if fdopen fails
	throw std::runtime_error("can't open file");
      }

      d_updated = true;
      d_repeat = repeat;
    }

    void
    file_source_impl::close()
    {
      // obtain exclusive access for duration of this function
      gruel::scoped_lock lock(fp_mutex);

      if(d_new_fp != NULL) {
	fclose(d_new_fp);
	d_new_fp = NULL;
      }
      d_updated = true;
    }

    void
    file_source_impl::do_update()
    {
      if(d_updated) {
	gruel::scoped_lock lock(fp_mutex); // hold while in scope

	if(d_fp)
	  fclose(d_fp);

	d_fp = d_new_fp;    // install new file pointer
	d_new_fp = 0;
	d_updated = false;
      }
    }

    int
    file_source_impl::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      char *o = (char*)output_items[0];
      int i;
      int size = noutput_items;

      do_update();       // update d_fp is reqd
      if(d_fp == NULL)
	throw std::runtime_error("work with file not open");

      gruel::scoped_lock lock(fp_mutex); // hold for the rest of this function
      while(size) {
	i = fread(o, d_itemsize, size, (FILE*)d_fp);

	size -= i;
	o += i * d_itemsize;

	if(size == 0)		// done
	  break;

	if(i > 0)			// short read, try again
	  continue;

	// We got a zero from fread.  This is either EOF or error.  In
	// any event, if we're in repeat mode, seek back to the beginning
	// of the file and try again, else break
	if(!d_repeat)
	  break;

	if(fseek ((FILE *) d_fp, 0, SEEK_SET) == -1) {
	  fprintf(stderr, "[%s] fseek failed\n", __FILE__);
	  exit(-1);
	}
      }

      if(size > 0) {	     		// EOF or error
	if(size == noutput_items)       // we didn't read anything; say we're done
	  return -1;
	return noutput_items - size;	// else return partial result
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
