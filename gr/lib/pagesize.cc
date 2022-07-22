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

#include "pagesize.h"
#include <gnuradio/logger.h>
#include <unistd.h>
#include <cstdio>

namespace gr {

#if defined(_WIN32) && defined(HAVE_GETPAGESIZE)
extern "C" size_t getpagesize(void);
#endif

int pagesize()
{
    static int s_pagesize = -1;
    gr::logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "pagesize");

    if (s_pagesize == -1) {
#if defined(HAVE_GETPAGESIZE)
        s_pagesize = getpagesize();
#elif defined(HAVE_SYSCONF)
        s_pagesize = sysconf(_SC_PAGESIZE);
        if (s_pagesize == -1) {
            logger->error("_SC_PAGESIZE: {}", strerror(errno));
            s_pagesize = 4096;
        }
#else
        logger->error("no info; setting pagesize = 4096");
        s_pagesize = 4096;
#endif
    }
    return s_pagesize;
}

} /* namespace gr */
