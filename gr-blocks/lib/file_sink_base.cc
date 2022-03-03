/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2007,2009,2013 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/blocks/file_sink_base.h>
#include <gnuradio/logger.h>
#include <gnuradio/thread/thread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <stdexcept>

// win32 (mingw/msvc) specific
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef O_BINARY
#define OUR_O_BINARY O_BINARY
#else
#define OUR_O_BINARY 0
#endif

// should be handled via configure
#ifdef O_LARGEFILE
#define OUR_O_LARGEFILE O_LARGEFILE
#else
#define OUR_O_LARGEFILE 0
#endif

namespace gr {
namespace blocks {

file_sink_base::file_sink_base(const char* filename, bool is_binary, bool append)
    : d_fp(0), d_new_fp(0), d_updated(false), d_is_binary(is_binary), d_append(append)
{
    gr::configure_default_loggers(d_base_logger, d_base_debug_logger, "file_sink_base");
    if (!open(filename))
        throw std::runtime_error("can't open file");
}

file_sink_base::~file_sink_base()
{
    d_base_debug_logger->debug("[destructor] Closing file");
    close();
    if (d_fp) {
        fclose(d_fp);
        d_fp = 0;
    }
}

bool file_sink_base::open(const char* filename)
{
    gr::thread::scoped_lock guard(d_mutex); // hold mutex for duration of this function
    d_base_debug_logger->debug("opening file {:s}", filename);

    // we use the open system call to get access to the O_LARGEFILE flag.
    int fd;
    int flags;

    if (d_append) {
        flags = O_WRONLY | O_CREAT | O_APPEND | OUR_O_LARGEFILE | OUR_O_BINARY;
    } else {
        flags = O_WRONLY | O_CREAT | O_TRUNC | OUR_O_LARGEFILE | OUR_O_BINARY;
    }
    if ((fd = ::open(filename, flags, 0664)) < 0) {
        d_base_logger->error("[::open] {:s}: {:s}", filename, strerror(errno));
        return false;
    }
    if (d_new_fp) { // if we've already got a new one open, close it
        fclose(d_new_fp);
        d_new_fp = 0;
    }

    if ((d_new_fp = fdopen(fd, d_is_binary ? "wb" : "w")) == NULL) {
        d_base_logger->error("[fdopen] {:s}: {:s}", filename, strerror(errno));
        ::close(fd); // don't leak file descriptor if fdopen fails.
    }

    d_updated = true;
    return d_new_fp != 0;
}

void file_sink_base::close()
{
    gr::thread::scoped_lock guard(d_mutex); // hold mutex for duration of this function

    d_base_debug_logger->debug("Closing file");
    if (d_new_fp) {
        fclose(d_new_fp);
        d_new_fp = 0;
    }
    d_updated = true;
}

void file_sink_base::do_update()
{
    if (d_updated) {
        gr::thread::scoped_lock guard(d_mutex); // hold mutex for duration of this block
        d_base_debug_logger->debug("updating");
        if (d_fp)
            fclose(d_fp);
        d_fp = d_new_fp; // install new file pointer
        d_new_fp = 0;
        d_updated = false;
    }
}

void file_sink_base::set_unbuffered(bool unbuffered)
{
    d_base_debug_logger->debug("Setting to {:s}buffered state", unbuffered ? "un" : "");
    d_unbuffered = unbuffered;
}

} /* namespace blocks */
} /* namespace gr */
