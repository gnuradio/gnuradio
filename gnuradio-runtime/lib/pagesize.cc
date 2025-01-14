/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 * Copyright 2023 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(_WIN32)
// always include <windows.h> first
#include <windows.h>

// Sysinfoapi.h MUST NOT BE included before windows.h
#include <Sysinfoapi.h>
#endif

#include "pagesize.h"
#include <gnuradio/logger.h>

namespace gr {

#if defined(_WIN32)

int native_pagesize(const gr::logger_ptr logger)
{
    SYSTEM_INFO win_sysinfo;
    GetNativeSystemInfo(&win_sysinfo);
    auto psize = win_sysinfo.dwPageSize;
    if (psize <= 0) {
        logger->error("GetNativeSystemInfo: Got invalid dwPageSize {}", psize);
        return -1;
    }
    return psize;
}

#elif defined(HAVE_GETPAGESIZE) //_WIN32 undefined,  getpagesize available

#include <unistd.h>

int native_pagesize(const gr::logger_ptr logger) { return getpagesize(); }

#elif defined(HAVE_SYSCONF) //_WIN32 undefined, sysconf available

#include <unistd.h>

int native_pagesize(const gr::logger_ptr logger)
{
    int pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize < 1) {
        logger->error("sysconf: _SC_PAGESIZE = {} < 1: {:s}", pagesize, strerror(errno));
        return -1;
    }
    return pagesize;
}

#else // neither _WIN32, nor getpagesize, nor sysconf

int native_pagesize(const gr::logger_ptr logger)
{
    logger->warning("No supported method of determining the pagesize available.");
    return -1;
}

#endif


int pagesize()
{
    static int s_pagesize = -1;

    if (s_pagesize == -1) {
        // only make a logger for the one time we actually intend to log anything.
        gr::logger_ptr logger, debug_logger;
        gr::configure_default_loggers(logger, debug_logger, "pagesize");
        s_pagesize = native_pagesize(logger);
        if (s_pagesize <= 0) {
            constexpr int default_pagesize = 4096;
            logger->error("Unable to determine page size. Using default of {} B instead.",
                          default_pagesize);
            s_pagesize = default_pagesize;
        }
        logger->debug("Setting pagesize to {} B", s_pagesize);
    }
    return s_pagesize;
}

} /* namespace gr */
