/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "file_descriptor_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <stdexcept>

#ifdef HAVE_IO_H
#include <io.h>
#endif

namespace gr {
namespace blocks {

file_descriptor_sink::sptr file_descriptor_sink::make(size_t itemsize, int fd)
{
    return gnuradio::make_block_sptr<file_descriptor_sink_impl>(itemsize, fd);
}

file_descriptor_sink_impl::file_descriptor_sink_impl(size_t itemsize, int fd)
    : sync_block("file_descriptor_sink",
                 io_signature::make(1, 1, itemsize),
                 io_signature::make(0, 0, 0)),
      d_itemsize(itemsize),
      d_fd(fd)
{
}

file_descriptor_sink_impl::~file_descriptor_sink_impl() { close(d_fd); }

int file_descriptor_sink_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    const char* inbuf = static_cast<const char*>(input_items[0]);
    unsigned long byte_size = noutput_items * d_itemsize;

    while (byte_size > 0) {
        auto r = write(d_fd, inbuf, byte_size);
        if (r == -1) {
            if (errno == EINTR)
                continue;
            else {
                d_logger->error("{:s}", strerror(errno));
                return -1; // indicate we're done
            }
        } else {
            byte_size -= r;
            inbuf += r;
        }
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
