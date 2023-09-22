/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_FILE_SINK_BASE_H
#define INCLUDED_GR_FILE_SINK_BASE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/logger.h>
#include <gnuradio/thread/thread.h>
#include <cstdio>

namespace gr {
namespace blocks {

/*!
 * \brief Common base class for file sinks
 */
class BLOCKS_API file_sink_base
{
protected:
    FILE* d_fp;     // current FILE pointer
    FILE* d_new_fp; // new FILE pointer
    bool d_updated; // is there a new FILE pointer?
    bool d_is_binary;
    gr::thread::mutex d_mutex;
    bool d_unbuffered;
    bool d_append;
    gr::logger_ptr d_base_logger;
    gr::logger_ptr d_base_debug_logger;

protected:
    file_sink_base(const char* filename, bool is_binary, bool append);

public:
    file_sink_base() {}
    ~file_sink_base();

    /*!
     * \brief Open filename and begin output to it.
     */
    bool open(const char* filename);

    /*!
     * \brief Close current output file.
     *
     * Closes current output file and ignores any output until
     * open is called to connect to another file.
     */
    void close();

    /*!
     * \brief if we've had an update, do it now.
     */
    void do_update();

    /*!
     * \brief turn on unbuffered writes for slower outputs
     */
    void set_unbuffered(bool unbuffered);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_SINK_BASE_H */
