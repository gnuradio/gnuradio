/* -*- c++ -*- */
/*
 * Copyright 2003,2011,2013 Free Software Foundation, Inc.
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

#include "vmcircbuf_mmap_tmpfile.h"
#include <stdexcept>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "pagesize.h"
#include <gnuradio/sys_paths.h>
#include <gnuradio/logger.h>

namespace gr {

  vmcircbuf_mmap_tmpfile::vmcircbuf_mmap_tmpfile (int size)
    : gr::vmcircbuf (size)
  {
#if !defined(HAVE_MMAP)
    GR_ERROR("gr_log", "gr::vmcircbuf_mmap_tmpfile: mmap or mkstemp is not available");
    throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
#else
    gr::thread::scoped_lock guard(s_vm_mutex);

    if(size <= 0 || (size % gr::pagesize ()) != 0) {
      GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile:  invalid size = %1%") % size);
      throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    int seg_fd = -1;
    char seg_name[1024];

    static int s_seg_counter = 0;

    // open a temporary file that we'll map in a bit later
    while(1) {
      snprintf(seg_name, sizeof(seg_name),
               "%s/gnuradio-%d-%d-XXXXXX", gr::tmp_path(), getpid(), s_seg_counter);
      s_seg_counter++;

#ifndef ANDROID
      seg_fd = open(seg_name, O_RDWR | O_CREAT | O_EXCL, 0600);
#else
      seg_fd = open(seg_name, O_RDWR | O_CREAT, 0600);
#endif /* ANDROID */

      GR_INFO("gr_log", boost::format("gr::mvcircbuf_mmap_tmpfile: opened: %1%") % seg_fd);

      if(seg_fd == -1) {
        if(errno == EEXIST) // File already exists (shouldn't happen).  Try again
          continue;

        char msg[1024];
        snprintf(msg, sizeof (msg),
                 "gr::vmcircbuf_mmap_tmpfile: open [%s]", seg_name);
        GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile: open failed [%1%], errno: %1%") \
                 % seg_name % (strerror(errno)));
        throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
      }
      break;
    }

    if(unlink (seg_name) == -1) {
      GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile: unlink, errno: %1%") % (strerror(errno)));
      throw std::runtime_error ("gr::vmcircbuf_mmap_tmpfile");
    }

    // We've got a valid file descriptor to a tmp file.
    // Now set it's length to 2x what we really want and mmap it in.
    if(ftruncate (seg_fd, (off_t) 2 * size) == -1) {
      close(seg_fd);						// cleanup
      GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile: ftruncate (1), errno: %1%") \
               % (strerror(errno)));
      throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    void *first_copy = mmap(0, 2 * size,
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            seg_fd, (off_t)0);

    if(first_copy == MAP_FAILED) {
      close(seg_fd);						// cleanup
      GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile: mmap (1), errno: %1%") % (strerror(errno)));
      throw std::runtime_error ("gr::vmcircbuf_mmap_tmpfile");
    }

    // unmap the 2nd half
    if(munmap ((char *) first_copy + size, size) == -1) {
      close(seg_fd);						// cleanup
      GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile: munmap (1), errno: %1%") % (strerror(errno)));
      throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    // map the first half into the now available hole where the
    // second half used to be.
    void *second_copy = mmap((char*)first_copy + size, size,
                             PROT_READ | PROT_WRITE, MAP_SHARED,
                             seg_fd, (off_t)0);

    if(second_copy == MAP_FAILED) {
      munmap(first_copy, size);					// cleanup
      close(seg_fd);
      GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile: mmap (2), errno: %1%") % (strerror(errno)));
      throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    // check for contiguity
    if((char*)second_copy != (char*)first_copy + size) {
      munmap(first_copy, size);					// cleanup
      munmap(second_copy, size);
      close(seg_fd);
      GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile: non-contiguous second copy, errno: %1%")  % (strerror(errno)));
      throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    // cut the tmp file down to size
    if(ftruncate (seg_fd, (off_t) size) == -1) {
      munmap(first_copy, size);					// cleanup
      munmap(second_copy, size);
      close(seg_fd);
      GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile: ftruncate (2), errno: %1%") % (strerror(errno)));
      throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    close(seg_fd);	// fd no longer needed.  The mapping is retained.

    // Now remember the important stuff

    d_base = (char*)first_copy;
    d_size = size;
#endif
  }

  vmcircbuf_mmap_tmpfile::~vmcircbuf_mmap_tmpfile()
  {
#if defined(HAVE_MMAP)
    gr::thread::scoped_lock guard(s_vm_mutex);

    if(munmap(d_base, 2 * d_size) == -1) {
      GR_ERROR("gr_log", boost::format("gr::vmcircbuf_mmap_tmpfile: munmap (2), errno: %1%") % (strerror(errno)));
    }
#endif
  }

  // ----------------------------------------------------------------
  //			The factory interface
  // ----------------------------------------------------------------

  gr::vmcircbuf_factory *vmcircbuf_mmap_tmpfile_factory::s_the_factory = 0;

  gr::vmcircbuf_factory *
  vmcircbuf_mmap_tmpfile_factory::singleton()
  {
    if(s_the_factory)
      return s_the_factory;

    s_the_factory = new gr::vmcircbuf_mmap_tmpfile_factory();
    return s_the_factory;
  }

  int
  vmcircbuf_mmap_tmpfile_factory::granularity()
  {
    return gr::pagesize();
  }

  gr::vmcircbuf *
  vmcircbuf_mmap_tmpfile_factory::make(int size)
  {
    try {
      return new vmcircbuf_mmap_tmpfile(size);
    }
    catch (...) {
      return 0;
    }
  }

} /* namespace gr */
