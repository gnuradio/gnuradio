/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
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

#include "vmcircbuf_sysv_shm.h"
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
#include "pagesize.h"

#define MAX_SYSV_SHM_ATTEMPTS 3

namespace gr {

  vmcircbuf_sysv_shm::vmcircbuf_sysv_shm(int size)
    : gr::vmcircbuf(size)
  {
#if !defined(HAVE_SYS_SHM_H)
    fprintf(stderr, "gr::vmcircbuf_sysv_shm: sysv shared memory is not available\n");
    throw std::runtime_error("gr::vmcircbuf_sysv_shm");
#else

    gr::thread::scoped_lock guard(s_vm_mutex);

    int pagesize = gr::pagesize();

    if(size <= 0 || (size % pagesize) != 0) {
      fprintf(stderr, "gr::vmcircbuf_sysv_shm: invalid size = %d\n", size);
      throw std::runtime_error("gr::vmcircbuf_sysv_shm");
    }

    // Attempt to allocate buffers (handle bad_alloc errors)
    int attempts_remain(MAX_SYSV_SHM_ATTEMPTS);
    while(attempts_remain-- > 0){

        int shmid_guard = -1;
        int shmid1 = -1;
        int shmid2 = -1;

        // We use this as a guard page.  We'll map it read-only on both ends of the buffer.
        // Ideally we'd map it no access, but I don't think that's possible with SysV
        if((shmid_guard = shmget(IPC_PRIVATE, pagesize, IPC_CREAT | 0400)) == -1) {
          perror("gr::vmcircbuf_sysv_shm: shmget (0)");
          continue;
        }

        if((shmid2 = shmget(IPC_PRIVATE, 2 * size + 2 * pagesize, IPC_CREAT | 0700)) == -1) {
          perror("gr::vmcircbuf_sysv_shm: shmget (1)");
          shmctl(shmid_guard, IPC_RMID, 0);
          continue;
        }

        if((shmid1 = shmget(IPC_PRIVATE, size, IPC_CREAT | 0700)) == -1) {
          perror("gr::vmcircbuf_sysv_shm: shmget (2)");
          shmctl(shmid_guard, IPC_RMID, 0);
          shmctl(shmid2, IPC_RMID, 0);
          continue;
        }

        void *first_copy = shmat (shmid2, 0, 0);
        if(first_copy == (void *) -1) {
          perror("gr::vmcircbuf_sysv_shm: shmat (1)");
          shmctl(shmid_guard, IPC_RMID, 0);
          shmctl(shmid2, IPC_RMID, 0);
          shmctl(shmid1, IPC_RMID, 0);
          continue;
        }

        shmctl(shmid2, IPC_RMID, 0);

        // There may be a race between our detach and attach.
        //
        // If the system allocates all shared memory segments at the same
        // virtual addresses in all processes and if the system allocates
        // some other segment to first_copy or first_copoy + size between
        // our detach and attach, the attaches below could fail [I've never
        // seen it fail for this reason].
        shmdt(first_copy);

        // first read-only guard page
        if(shmat(shmid_guard, first_copy, SHM_RDONLY) == (void *) -1) {
          perror("gr::vmcircbuf_sysv_shm: shmat (2)");
          shmctl(shmid_guard, IPC_RMID, 0);
          shmctl(shmid1, IPC_RMID, 0);
          continue;
        }

        // first copy
        if(shmat (shmid1, (char*)first_copy + pagesize, 0) == (void *) -1) {
          perror("gr::vmcircbuf_sysv_shm: shmat (3)");
          shmctl(shmid_guard, IPC_RMID, 0);
          shmctl(shmid1, IPC_RMID, 0);
          shmdt(first_copy);
          continue;
        }

        // second copy
        if(shmat (shmid1, (char*)first_copy + pagesize + size, 0) == (void *) -1) {
          perror("gr::vmcircbuf_sysv_shm: shmat (4)");
          shmctl(shmid_guard, IPC_RMID, 0);
          shmctl(shmid1, IPC_RMID, 0);
          shmdt((char *)first_copy + pagesize);
          continue;
        }

        // second read-only guard page
        if(shmat(shmid_guard, (char*)first_copy + pagesize + 2 * size, SHM_RDONLY) == (void *) -1) {
          perror("gr::vmcircbuf_sysv_shm: shmat (5)");
          shmctl(shmid_guard, IPC_RMID, 0);
          shmctl(shmid1, IPC_RMID, 0);
          shmdt(first_copy);
          shmdt((char *)first_copy + pagesize);
          shmdt((char *)first_copy + pagesize + size);
          continue;
        }

        shmctl(shmid1, IPC_RMID, 0);
        shmctl(shmid_guard, IPC_RMID, 0);

        // Now remember the important stuff
        d_base = (char*)first_copy + pagesize;
        d_size = size;
        break;
    }
    if(attempts_remain<0){
        throw std::runtime_error("gr::vmcircbuf_sysv_shm");
    }
#endif
  }

  vmcircbuf_sysv_shm::~vmcircbuf_sysv_shm()
  {
#if defined(HAVE_SYS_SHM_H)
    gr::thread::scoped_lock guard(s_vm_mutex);

    if(shmdt(d_base - gr::pagesize()) == -1
       || shmdt(d_base) == -1
       || shmdt(d_base + d_size) == -1
       || shmdt(d_base + 2 * d_size) == -1){
      perror("gr::vmcircbuf_sysv_shm: shmdt (2)");
    }
#endif
  }

  // ----------------------------------------------------------------
  //			The factory interface
  // ----------------------------------------------------------------

  gr::vmcircbuf_factory *vmcircbuf_sysv_shm_factory::s_the_factory = 0;

  gr::vmcircbuf_factory *
  vmcircbuf_sysv_shm_factory::singleton()
  {
    if(s_the_factory)
      return s_the_factory;

    s_the_factory = new gr::vmcircbuf_sysv_shm_factory();
    return s_the_factory;
  }

  int
  vmcircbuf_sysv_shm_factory::granularity()
  {
    return gr::pagesize();
  }

  gr::vmcircbuf *
  vmcircbuf_sysv_shm_factory::make(int size)
  {
    try {
      return new vmcircbuf_sysv_shm(size);
    }
    catch (...) {
      return 0;
    }
  }

} /* namespace gr */
