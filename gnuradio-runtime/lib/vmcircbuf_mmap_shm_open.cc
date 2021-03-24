/* -*- c++ -*- */
/*
 * Copyright 2003,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "vmcircbuf_mmap_shm_open.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include "pagesize.h"
#include <gnuradio/sys_paths.h>
#include <boost/format.hpp>
#include <cerrno>
#include <cstdio>

namespace gr {

vmcircbuf_mmap_shm_open::vmcircbuf_mmap_shm_open(size_t size) : gr::vmcircbuf(size)
{
#if !defined(HAVE_MMAP) || !defined(HAVE_SHM_OPEN)
    std::stringstream error_msg;
    error_msg << "mmap or shm_open is not available";
    GR_LOG_ERROR(d_logger, error_msg.str());
    throw std::runtime_error("gr::vmcircbuf_mmap_shm_open");
#else
    gr::thread::scoped_lock guard(s_vm_mutex);

    static int s_seg_counter = 0;

    if (size <= 0 || (size % gr::pagesize()) != 0) {
        GR_LOG_ERROR(d_logger, "invalid size =" + std::to_string(size));
        throw std::runtime_error("gr::vmcircbuf_mmap_shm_open");
    }

    int shm_fd = -1;
    std::string seg_name;
    static bool portable_format = true;

    // open a new named shared memory segment
    while (1) {
        if (portable_format) {

            // This is the POSIX recommended "portable format".
            // Of course the "portable format" doesn't work on some systems...
            seg_name = str(boost::format("/gnuradio-%d-%d") % getpid() % s_seg_counter);
        } else {

            // Where the "portable format" doesn't work, we try building
            // a full filesystem pathname pointing into a suitable temporary directory.

            seg_name = str(boost::format("%s/gnuradio-%d-%d") % gr::tmp_path() %
                           getpid() % s_seg_counter);
        }

        shm_fd = shm_open(seg_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
        if (shm_fd == -1 && errno == EACCES && portable_format) {
            portable_format = false;
            continue; // try again using "non-portable format"
        }

        s_seg_counter++;

        if (shm_fd == -1) {
            if (errno ==
                EEXIST) // Named segment already exists (shouldn't happen).  Try again
                continue;

            static std::string msg =
                str(boost::format("shm_open [%s] failed") % seg_name);
            GR_LOG_ERROR(d_logger, msg.c_str());
            throw std::runtime_error("gr::vmcircbuf_mmap_shm_open");
        }
        break;
    }

    // We've got a new shared memory segment fd open.
    // Now set it's length to 2x what we really want and mmap it in.
    if (ftruncate(shm_fd, (off_t)2 * size) == -1) {
        close(shm_fd); // cleanup
        GR_LOG_ERROR(d_logger, "ftruncate failed");
        throw std::runtime_error("gr::vmcircbuf_mmap_shm_open");
    }

    void* first_copy =
        mmap(0, 2 * size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, (off_t)0);

    if (first_copy == MAP_FAILED) {
        close(shm_fd); // cleanup
        GR_LOG_ERROR(d_logger, "mmap (1) failed");
        throw std::runtime_error("gr::vmcircbuf_mmap_shm_open");
    }

    // unmap the 2nd half
    if (munmap((char*)first_copy + size, size) == -1) {
        close(shm_fd); // cleanup
        GR_LOG_ERROR(d_logger, "munmap (1) failed");
        throw std::runtime_error("gr::vmcircbuf_mmap_shm_open");
    }

    // map the first half into the now available hole where the
    // second half used to be.
    void* second_copy = mmap((char*)first_copy + size,
                             size,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED,
                             shm_fd,
                             (off_t)0);

    if (second_copy == MAP_FAILED) {
        close(shm_fd); // cleanup
        GR_LOG_ERROR(d_logger, "mmap (2) failed");
        throw std::runtime_error("gr::vmcircbuf_mmap_shm_open");
    }

#if 0 // OS/X doesn't allow you to resize the segment

    // cut the shared memory segment down to size
    if(ftruncate(shm_fd, (off_t)size) == -1) {
      close(shm_fd);						// cleanup
      perror("gr::vmcircbuf_mmap_shm_open: ftruncate (2)");
      throw std::runtime_error("gr::vmcircbuf_mmap_shm_open");
    }
#endif

    close(shm_fd); // fd no longer needed.  The mapping is retained.

    if (shm_unlink(seg_name.c_str()) == -1) { // unlink the seg_name.
        GR_LOG_ERROR(d_logger, "shm_unlink failed");
        throw std::runtime_error("gr::vmcircbuf_mmap_shm_open");
    }

    // Now remember the important stuff
    d_base = (char*)first_copy;
    d_size = size;
#endif
}

vmcircbuf_mmap_shm_open::~vmcircbuf_mmap_shm_open()
{
#if defined(HAVE_MMAP)
    gr::thread::scoped_lock guard(s_vm_mutex);

    if (munmap(d_base, 2 * d_size) == -1) {
        GR_LOG_ERROR(d_logger, "munmap (2) failed");
    }
#endif
}

// ----------------------------------------------------------------
//			The factory interface
// ----------------------------------------------------------------

gr::vmcircbuf_factory* vmcircbuf_mmap_shm_open_factory::s_the_factory = 0;

gr::vmcircbuf_factory* vmcircbuf_mmap_shm_open_factory::singleton()
{
    if (s_the_factory)
        return s_the_factory;

    s_the_factory = new gr::vmcircbuf_mmap_shm_open_factory();
    return s_the_factory;
}

int vmcircbuf_mmap_shm_open_factory::granularity() { return gr::pagesize(); }

gr::vmcircbuf* vmcircbuf_mmap_shm_open_factory::make(size_t size)
{
    try {
        return new vmcircbuf_mmap_shm_open(size);
    } catch (...) {
        return 0;
    }
}

} /* namespace gr */
