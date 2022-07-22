/* -*- c++ -*- */
/*
 * Copyright 2016,2019 Free Software Foundation, Inc.
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "zmq_common_impl.h"
#include <gnuradio/block_work_io.h>
#include <gnuradio/logger.h>
#include <gnuradio/tag.h>

namespace gr {
namespace zeromq {

class base
{
public:
    base(int type,
         size_t itemsize,
         int timeout,
         bool pass_tags,
         const std::string& key = "");

    void set_vsize(size_t vsize) { d_vsize = vsize; }

protected:
    std::string last_endpoint() const;
    zmq::context_t d_context;
    zmq::socket_t d_socket;
    size_t d_vsize;
    int d_timeout;
    bool d_pass_tags;
    const std::string d_key;

    logger_ptr d_base_logger;
    logger_ptr d_base_debug_logger;
};

class base_sink : public base
{
public:
    base_sink(int type,
              size_t itemsize,
              const std::string& address,
              int timeout,
              bool pass_tags,
              int hwm,
              const std::string& key = "");

protected:
    int send_message(const void* in_buf,
                     const int in_nitems,
                     const uint64_t in_offset,
                     const std::vector<tag_t>& tags);
};

class base_source : public base
{
public:
    base_source(int type,
                size_t itemsize,
                const std::string& address,
                int timeout,
                bool pass_tags,
                int hwm,
                const std::string& key = "");

protected:
    zmq::message_t d_msg;
    std::vector<gr::tag_t> d_tags;
    size_t d_consumed_bytes;
    int d_consumed_items;

    bool has_pending();
    int flush_pending(block_work_output& work_output,
                      const int out_nitems,
                      const uint64_t out_offset);
    bool load_message(bool wait);
};

} // namespace zeromq
} // namespace gr
