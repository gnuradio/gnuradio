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

#include "vmcircbuf_mmap_tmpfile.h"
#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include "pagesize.h"
#include <gnuradio/sys_paths.h>
#include <fcntl.h>
#include <spdlog/fmt/fmt.h>
#include <cerrno>
#include <cstring>

namespace gr {

vmcircbuf_mmap_tmpfile::vmcircbuf_mmap_tmpfile(size_t size) : gr::vmcircbuf(size)
{
#if !defined(HAVE_MMAP)
    d_logger->error("mmap or mkstemp is not available");
    throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
#else
    gr::thread::scoped_lock guard(s_vm_mutex);

    if (size <= 0 || (size % gr::pagesize()) != 0) {
        d_logger->error("invalid size = {:d}", size);
        throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    int seg_fd = -1;

    static int s_seg_counter = 0;

    std::filesystem::path seg_name;
    // open a temporary file that we'll map in a bit later
    while (1) {
        seg_name = gr::paths::tmp() /
                   fmt::format("gnuradio-{}-{}-XXXXXX", getpid(), s_seg_counter);

        s_seg_counter++;

        seg_fd = open(seg_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
        if (seg_fd == -1) {
            if (errno == EEXIST) // File already exists (shouldn't happen).  Try again
                continue;

            d_logger->error("open [{:s}]", seg_name.string());
            throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
        }
        break;
    }

    if (unlink(seg_name.c_str()) == -1) {
        d_logger->error("unlink");
        throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    // We've got a valid file descriptor to a tmp file.
    // Now set it's length to 2x what we really want and mmap it in.
    if (ftruncate(seg_fd, (off_t)2 * size) == -1) {
        close(seg_fd); // cleanup
        d_logger->error("ftruncate (1) failed");
        throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    void* first_copy =
        mmap(0, 2 * size, PROT_READ | PROT_WRITE, MAP_SHARED, seg_fd, (off_t)0);

    if (first_copy == MAP_FAILED) {
        close(seg_fd); // cleanup
        d_logger->error("mmap (1) failed");
        throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    // map the first half into the second half of the address space.
    void* second_copy = mmap((char*)first_copy + size,
                             size,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED | MAP_FIXED,
                             seg_fd,
                             (off_t)0);

    if (second_copy == MAP_FAILED) {
        munmap(first_copy, size); // cleanup
        close(seg_fd);
        d_logger->error("mmap (2) failed");
        throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    // cut the tmp file down to size
    if (ftruncate(seg_fd, (off_t)size) == -1) {
        munmap(first_copy, size); // cleanup
        munmap(second_copy, size);
        close(seg_fd);
        d_logger->error("ftruncate (2) failed");
        throw std::runtime_error("gr::vmcircbuf_mmap_tmpfile");
    }

    close(seg_fd); // fd no longer needed.  The mapping is retained.

    // Now remember the important stuff

    d_base = (char*)first_copy;
    d_size = size;
#endif
}

vmcircbuf_mmap_tmpfile::~vmcircbuf_mmap_tmpfile()
{
#if defined(HAVE_MMAP)
    gr::thread::scoped_lock guard(s_vm_mutex);

    if (munmap(d_base, 2 * d_size) == -1) {
        d_logger->error("munmap (2) failed");
    }
#endif
}

// ----------------------------------------------------------------
//			The factory interface
// ----------------------------------------------------------------

gr::vmcircbuf_factory* vmcircbuf_mmap_tmpfile_factory::s_the_factory = 0;

gr::vmcircbuf_factory* vmcircbuf_mmap_tmpfile_factory::singleton()
{
    if (s_the_factory)
        return s_the_factory;

    s_the_factory = new gr::vmcircbuf_mmap_tmpfile_factory();
    return s_the_factory;
}

int vmcircbuf_mmap_tmpfile_factory::granularity() { return gr::pagesize(); }

gr::vmcircbuf* vmcircbuf_mmap_tmpfile_factory::make(size_t size)
{
    try {
        return new vmcircbuf_mmap_tmpfile(size);
    } catch (...) {
        return 0;
    }
}

} /* namespace gr */
