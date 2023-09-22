/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "vmcircbuf_sysv_shm.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif
#ifdef HAVE_SYS_SHM_H
#include <sys/shm.h>
#endif
#include "pagesize.h"
#include <cerrno>

#define MAX_SYSV_SHM_ATTEMPTS 3

namespace gr {

vmcircbuf_sysv_shm::vmcircbuf_sysv_shm(size_t size) : gr::vmcircbuf(size)
{
#if !defined(HAVE_SYS_SHM_H)
    d_logger->error("sysv shared memory is not available");
    throw std::runtime_error("gr::vmcircbuf_sysv_shm");
#else

    gr::thread::scoped_lock guard(s_vm_mutex);

    int pagesize = gr::pagesize();

    if (size <= 0 || (size % pagesize) != 0) {
        d_logger->error("invalid size = {:d}", size);
        throw std::runtime_error("gr::vmcircbuf_sysv_shm");
    }

    // Attempt to allocate buffers (handle bad_alloc errors)
    int attempts_remain(MAX_SYSV_SHM_ATTEMPTS);
    while (attempts_remain-- > 0) {

        int shmid_guard = -1;
        int shmid1 = -1;
        int shmid2 = -1;

        // We use this as a guard page.  We'll map it read-only on both ends of the
        // buffer. Ideally we'd map it no access, but I don't think that's possible with
        // SysV
        if ((shmid_guard = shmget(IPC_PRIVATE, pagesize, IPC_CREAT | 0400)) == -1) {
            d_logger->error("shmget (0): {:s}", strerror(errno));
            continue;
        }

        if ((shmid2 = shmget(IPC_PRIVATE, 2 * size + 2 * pagesize, IPC_CREAT | 0700)) ==
            -1) {
            d_logger->error("shmget (1): {:s}", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            continue;
        }

        if ((shmid1 = shmget(IPC_PRIVATE, size, IPC_CREAT | 0700)) == -1) {
            d_logger->error("shmget (2): {:s}", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid2, IPC_RMID, 0);
            continue;
        }

        void* first_copy = shmat(shmid2, 0, 0);
        if (first_copy == (void*)-1) {
            d_logger->error("shmat (1): {:s}", strerror(errno));
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
        if (shmat(shmid_guard, first_copy, SHM_RDONLY) == (void*)-1) {
            d_logger->error("shmat (2): {:s}", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid1, IPC_RMID, 0);
            continue;
        }

        // first copy
        if (shmat(shmid1, (char*)first_copy + pagesize, 0) == (void*)-1) {
            d_logger->error("shmat (3): {:s}", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid1, IPC_RMID, 0);
            shmdt(first_copy);
            continue;
        }

        // second copy
        if (shmat(shmid1, (char*)first_copy + pagesize + size, 0) == (void*)-1) {
            d_logger->error("shmat (4): {:s}", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid1, IPC_RMID, 0);
            shmdt((char*)first_copy + pagesize);
            continue;
        }

        // second read-only guard page
        if (shmat(shmid_guard, (char*)first_copy + pagesize + 2 * size, SHM_RDONLY) ==
            (void*)-1) {
            d_logger->error("shmat (5): {:s}", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid1, IPC_RMID, 0);
            shmdt(first_copy);
            shmdt((char*)first_copy + pagesize);
            shmdt((char*)first_copy + pagesize + size);
            continue;
        }

        shmctl(shmid1, IPC_RMID, 0);
        shmctl(shmid_guard, IPC_RMID, 0);

        // Now remember the important stuff
        d_base = (char*)first_copy + pagesize;
        d_size = size;
        break;
    }
    if (attempts_remain < 0) {
        throw std::runtime_error("gr::vmcircbuf_sysv_shm");
    }
#endif
}

vmcircbuf_sysv_shm::~vmcircbuf_sysv_shm()
{
#if defined(HAVE_SYS_SHM_H)
    gr::thread::scoped_lock guard(s_vm_mutex);

    if (shmdt(d_base - gr::pagesize()) == -1 || shmdt(d_base) == -1 ||
        shmdt(d_base + d_size) == -1 || shmdt(d_base + 2 * d_size) == -1) {
        d_logger->error("shmdt (2): {:s}", strerror(errno));
    }
#endif
}

// ----------------------------------------------------------------
//			The factory interface
// ----------------------------------------------------------------

gr::vmcircbuf_factory* vmcircbuf_sysv_shm_factory::s_the_factory = 0;

gr::vmcircbuf_factory* vmcircbuf_sysv_shm_factory::singleton()
{
    if (s_the_factory)
        return s_the_factory;

    s_the_factory = new gr::vmcircbuf_sysv_shm_factory();
    return s_the_factory;
}

int vmcircbuf_sysv_shm_factory::granularity() { return gr::pagesize(); }

gr::vmcircbuf* vmcircbuf_sysv_shm_factory::make(size_t size)
{
    try {
        return new vmcircbuf_sysv_shm(size);
    } catch (...) {
        return 0;
    }
}

} /* namespace gr */
