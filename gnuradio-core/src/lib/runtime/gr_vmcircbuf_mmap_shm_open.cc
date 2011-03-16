/* -*- c++ -*- */
/*
 * Copyright 2003,2011 Free Software Foundation, Inc.
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
#include <gr_vmcircbuf_mmap_shm_open.h>
#include <stdexcept>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <gr_pagesize.h>
#include <gr_sys_paths.h>


gr_vmcircbuf_mmap_shm_open::gr_vmcircbuf_mmap_shm_open (int size)
  : gr_vmcircbuf (size)
{
#if !defined(HAVE_MMAP) || !defined(HAVE_SHM_OPEN)
  fprintf (stderr, "gr_vmcircbuf_mmap_shm_open: mmap or shm_open is not available\n");
  throw std::runtime_error ("gr_vmcircbuf_mmap_shm_open");
#else
  static int s_seg_counter = 0;

  if (size <= 0 || (size % gr_pagesize ()) != 0){
    fprintf (stderr, "gr_vmcircbuf_mmap_shm_open: invalid size = %d\n", size);
    throw std::runtime_error ("gr_vmcircbuf_mmap_shm_open");
  }

  int		shm_fd = -1;
  char	 	seg_name[1024];
  static bool	portable_format = true;
  
  // open a new named shared memory segment

  while (1){
    if (portable_format){

      // This is the POSIX recommended "portable format".
      // Of course the "portable format" doesn't work on some systems...
      
      snprintf (seg_name, sizeof (seg_name),
		"/gnuradio-%d-%d", getpid (), s_seg_counter);
    }
    else {

      // Where the "portable format" doesn't work, we try building
      // a full filesystem pathname pointing into a suitable temporary directory.
      
      snprintf (seg_name, sizeof (seg_name),
		"%s/gnuradio-%d-%d", gr_tmp_path (), getpid (), s_seg_counter);
    }

    shm_fd = shm_open (seg_name, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (shm_fd == -1 && errno == EACCES && portable_format){
      portable_format = false;
      continue;			// try again using "non-portable format"
    }

    s_seg_counter++;

    if (shm_fd == -1){
      if (errno == EEXIST)	// Named segment already exists (shouldn't happen).  Try again
	continue;

      char msg[1024];
      snprintf (msg, sizeof (msg), "gr_vmcircbuf_mmap_shm_open: shm_open [%s]", seg_name);
      perror (msg);
      throw std::runtime_error ("gr_vmcircbuf_mmap_shm_open");
    }
    break;
  }

  // We've got a new shared memory segment fd open.
  // Now set it's length to 2x what we really want and mmap it in.

  if (ftruncate (shm_fd, (off_t) 2 * size) == -1){
    close (shm_fd);						// cleanup
    perror ("gr_vmcircbuf_mmap_shm_open: ftruncate (1)");
    throw std::runtime_error ("gr_vmcircbuf_mmap_shm_open");
  }

  void *first_copy = mmap (0, 2 * size,
			   PROT_READ | PROT_WRITE, MAP_SHARED,
			   shm_fd, (off_t) 0);

  if (first_copy == MAP_FAILED){
    close (shm_fd);						// cleanup
    perror ("gr_vmcircbuf_mmap_shm_open: mmap (1)");
    throw std::runtime_error ("gr_vmcircbuf_mmap_shm_open");
  }

  // unmap the 2nd half
  if (munmap ((char *) first_copy + size, size) == -1){
    close (shm_fd);						// cleanup
    perror ("gr_vmcircbuf_mmap_shm_open: munmap (1)");
    throw std::runtime_error ("gr_vmcircbuf_mmap_shm_open");
  }

  // map the first half into the now available hole where the
  // second half used to be.

  void *second_copy = mmap ((char *) first_copy + size, size,
			    PROT_READ | PROT_WRITE, MAP_SHARED,
			    shm_fd, (off_t) 0);

  if (second_copy == MAP_FAILED){
    close (shm_fd);						// cleanup
    perror ("gr_vmcircbuf_mmap_shm_open: mmap (2)");
    throw std::runtime_error ("gr_vmcircbuf_mmap_shm_open");
  }

#if 0  // OS/X doesn't allow you to resize the segment

  // cut the shared memory segment down to size
  if (ftruncate (shm_fd, (off_t) size) == -1){
    close (shm_fd);						// cleanup
    perror ("gr_vmcircbuf_mmap_shm_open: ftruncate (2)");
    throw std::runtime_error ("gr_vmcircbuf_mmap_shm_open");
  }
#endif

  close (shm_fd);	// fd no longer needed.  The mapping is retained.

  if (shm_unlink (seg_name) == -1){	// unlink the seg_name.  
    perror ("gr_vmcircbuf_mmap_shm_open: shm_unlink");
    throw std::runtime_error ("gr_vmcircbuf_mmap_shm_open");
  }

  // Now remember the important stuff

  d_base = (char *) first_copy;
  d_size = size;
#endif
}

gr_vmcircbuf_mmap_shm_open::~gr_vmcircbuf_mmap_shm_open ()
{
#if defined(HAVE_MMAP)  
  if (munmap (d_base, 2 * d_size) == -1){
    perror ("gr_vmcircbuf_mmap_shm_open: munmap (2)");
  }
#endif
}

// ----------------------------------------------------------------
//			The factory interface
// ----------------------------------------------------------------


gr_vmcircbuf_factory *gr_vmcircbuf_mmap_shm_open_factory::s_the_factory = 0;

gr_vmcircbuf_factory *
gr_vmcircbuf_mmap_shm_open_factory::singleton ()
{
  if (s_the_factory)
    return s_the_factory;

  s_the_factory = new gr_vmcircbuf_mmap_shm_open_factory ();
  return s_the_factory;
}

int
gr_vmcircbuf_mmap_shm_open_factory::granularity ()
{
  return gr_pagesize ();
}

gr_vmcircbuf *
gr_vmcircbuf_mmap_shm_open_factory::make (int size)
{
  try {
    return new gr_vmcircbuf_mmap_shm_open (size);
  }
  catch (...){
    return 0;
  }
}
