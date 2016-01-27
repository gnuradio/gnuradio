/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ZEROMQ_TAG_HEADERS_H
#define ZEROMQ_TAG_HEADERS_H

#include <gnuradio/io_signature.h>
#include <gnuradio/block.h>
#include <sstream>
#include <cstring>
#include <zmq.hpp>

namespace gr {
  namespace zeromq {

    std::string gen_tag_header(uint64_t offset, std::vector<gr::tag_t> &tags);
    size_t parse_tag_header(zmq::message_t &msg, uint64_t &offset_out, std::vector<gr::tag_t> &tags_out);
    
  } /* namespace zeromq */
} /* namespace gr */

#endif /* ZEROMQ_TAG_HEADERS_H */
