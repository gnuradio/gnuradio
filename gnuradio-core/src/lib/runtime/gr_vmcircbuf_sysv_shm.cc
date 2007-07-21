/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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
#include <gr_vmcircbuf_sysv_shm.h>
#include <stdexcept>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif
#ifdef HAVE_SYS_SHM_H
#include <sys/shm.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <gr_pagesize.h>


gr_vmcircbuf_sysv_shm::gr_vmcircbuf_sysv_shm (int size)
  : gr_vmcircbuf (size)
{
#if !defined(HAVE_SYS_SHM_H)
  fprintf (stderr, "gr_vmcircbuf_sysv_shm: sysv shared memory is not available\n");
  throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
#else

  int pagesize = gr_pagesize();

  if (size <= 0 || (size % pagesize) != 0){
    fprintf (stderr, "gr_vmcircbuf_sysv_shm: invalid size = %d\n", size);
    throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
  }

  int shmid_guard = -1;
  int shmid1 = -1;
  int shmid2 = -1;

  // We use this as a guard page.  We'll map it read-only on both ends of the buffer.
  // Ideally we'd map it no access, but I don't think that's possible with SysV
  if ((shmid_guard = shmget (IPC_PRIVATE, pagesize, IPC_CREAT | 0400)) == -1){
    perror ("gr_vmcircbuf_sysv_shm: shmget (0)");
    throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
  }

  if ((shmid2 = shmget (IPC_PRIVATE, 2 * size + 2 * pagesize, IPC_CREAT | 0700)) == -1){
    perror ("gr_vmcircbuf_sysv_shm: shmget (1)");
    shmctl (shmid_guard, IPC_RMID, 0);
    throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
  }

  if ((shmid1 = shmget (IPC_PRIVATE, size, IPC_CREAT | 0700)) == -1){
    perror ("gr_vmcircbuf_sysv_shm: shmget (2)");
    shmctl (shmid_guard, IPC_RMID, 0);
    shmctl (shmid2, IPC_RMID, 0);
    throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
  }

  void *first_copy = shmat (shmid2, 0, 0);
  if (first_copy == (void *) -1){
    perror ("gr_vmcircbuf_sysv_shm: shmat (1)");
    shmctl (shmid_guard, IPC_RMID, 0);
    shmctl (shmid2, IPC_RMID, 0);
    shmctl (shmid1, IPC_RMID, 0);
    throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
  }

  shmctl (shmid2, IPC_RMID, 0);

  // There may be a race between our detach and attach.
  //
  // If the system allocates all shared memory segments at the same
  // virtual addresses in all processes and if the system allocates
  // some other segment to first_copy or first_copoy + size between
  // our detach and attach, the attaches below could fail [I've never
  // seen it fail for this reason].
  
  shmdt (first_copy);

  // first read-only guard page
  if (shmat (shmid_guard, first_copy, SHM_RDONLY) == (void *) -1){
    perror ("gr_vmcircbuf_sysv_shm: shmat (2)");
    shmctl (shmid_guard, IPC_RMID, 0);
    shmctl (shmid1, IPC_RMID, 0);
    throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
  }

  // first copy
  if (shmat (shmid1, (char *) first_copy + pagesize, 0) == (void *) -1){
    perror ("gr_vmcircbuf_sysv_shm: shmat (3)");
    shmctl (shmid_guard, IPC_RMID, 0);
    shmctl (shmid1, IPC_RMID, 0);
    shmdt (first_copy);
    throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
  }

  // second copy
  if (shmat (shmid1, (char *) first_copy + pagesize + size, 0) == (void *) -1){
    perror ("gr_vmcircbuf_sysv_shm: shmat (4)");
    shmctl (shmid_guard, IPC_RMID, 0);
    shmctl (shmid1, IPC_RMID, 0);
    shmdt ((char *)first_copy + pagesize);
    throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
  }

  // second read-only guard page
  if (shmat (shmid_guard, (char *) first_copy + pagesize + 2 * size, SHM_RDONLY) == (void *) -1){
    perror ("gr_vmcircbuf_sysv_shm: shmat (5)");
    shmctl (shmid_guard, IPC_RMID, 0);
    shmctl (shmid1, IPC_RMID, 0);
    shmdt (first_copy);
    shmdt ((char *)first_copy + pagesize);
    shmdt ((char *)first_copy + pagesize + size);
    throw std::runtime_error ("gr_vmcircbuf_sysv_shm");
  }

  shmctl (shmid1, IPC_RMID, 0);
  shmctl (shmid_guard, IPC_RMID, 0);

  // Now remember the important stuff

  d_base = (char *) first_copy + pagesize;
  d_size = size;
#endif
}

gr_vmcircbuf_sysv_shm::~gr_vmcircbuf_sysv_shm ()
{
#if defined(HAVE_SYS_SHM_H)
  if (shmdt (d_base - gr_pagesize()) == -1
      || shmdt (d_base) == -1
      || shmdt (d_base + d_size) == -1
      || shmdt (d_base + 2 * d_size) == -1){
    perror ("gr_vmcircbuf_sysv_shm: shmdt (2)");
  }
#endif
}

// ----------------------------------------------------------------
//			The factory interface
// ----------------------------------------------------------------


gr_vmcircbuf_factory *gr_vmcircbuf_sysv_shm_factory::s_the_factory = 0;

gr_vmcircbuf_factory *
gr_vmcircbuf_sysv_shm_factory::singleton ()
{
  if (s_the_factory)
    return s_the_factory;

  s_the_factory = new gr_vmcircbuf_sysv_shm_factory ();
  return s_the_factory;
}

int
gr_vmcircbuf_sysv_shm_factory::granularity ()
{
  return gr_pagesize ();
}

gr_vmcircbuf *
gr_vmcircbuf_sysv_shm_factory::make (int size)
{
  try {
    return new gr_vmcircbuf_sysv_shm (size);
  }
  catch (...){
    return 0;
  }
}
