/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ZEROMQ_TAG_HEADERS_H
#define ZEROMQ_TAG_HEADERS_H

#include "zmq_common_impl.h"
#include <gnuradio/block.h>
#include <gnuradio/io_signature.h>
#include <cstring>
#include <sstream>

namespace gr {
namespace zeromq {

std::string gen_tag_header(uint64_t offset, std::vector<gr::tag_t>& tags);
size_t parse_tag_header(zmq::message_t& msg,
                        uint64_t& offset_out,
                        std::vector<gr::tag_t>& tags_out);

} /* namespace zeromq */
} /* namespace gr */

#endif /* ZEROMQ_TAG_HEADERS_H */
