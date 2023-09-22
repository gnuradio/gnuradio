/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "zmq_common_impl.h"
#include <gnuradio/block.h>
#include <gnuradio/io_signature.h>
#include <cstring>
#include <sstream>

#define GR_HEADER_MAGIC 0x5FF0
#define GR_HEADER_VERSION 0x01

namespace gr {
namespace zeromq {

struct membuf : std::streambuf {
    membuf(void* b, size_t len)
    {
        char* bc = static_cast<char*>(b);
        this->setg(bc, bc, bc + len);
    }
};

std::string gen_tag_header(uint64_t offset, std::vector<gr::tag_t>& tags)
{
    std::stringbuf sb("");
    std::ostream ss(&sb);

    uint16_t header_magic = GR_HEADER_MAGIC;
    uint8_t header_version = GR_HEADER_VERSION;
    uint64_t ntags = (uint64_t)tags.size();

    ss.write((const char*)&header_magic, sizeof(uint16_t));
    ss.write((const char*)&header_version, sizeof(uint8_t));
    ss.write((const char*)&offset, sizeof(uint64_t));
    ss.write((const char*)&ntags, sizeof(uint64_t));

    for (size_t i = 0; i < tags.size(); i++) {
        ss.write((const char*)&tags[i].offset, sizeof(uint64_t));
        pmt::serialize(tags[i].key, sb);
        pmt::serialize(tags[i].value, sb);
        pmt::serialize(tags[i].srcid, sb);
    }

    return sb.str();
}

size_t parse_tag_header(zmq::message_t& msg,
                        uint64_t& offset_out,
                        std::vector<gr::tag_t>& tags_out)
{
    membuf sb(msg.data(), msg.size());
    std::istream iss(&sb);

    size_t min_len =
        sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint64_t) + sizeof(uint64_t);
    if (msg.size() < min_len)
        throw std::runtime_error("incoming zmq msg too small to hold gr tag header!");

    uint16_t header_magic;
    uint8_t header_version;
    uint64_t rcv_ntags;

    iss.read((char*)&header_magic, sizeof(uint16_t));
    iss.read((char*)&header_version, sizeof(uint8_t));

    if (header_magic != GR_HEADER_MAGIC)
        throw std::runtime_error("gr header magic does not match!");

    if (header_version != 1)
        throw std::runtime_error("gr header version too high!");

    iss.read((char*)&offset_out, sizeof(uint64_t));
    iss.read((char*)&rcv_ntags, sizeof(uint64_t));

    for (size_t i = 0; i < rcv_ntags; i++) {
        gr::tag_t newtag;
        sb.sgetn((char*)&(newtag.offset), sizeof(uint64_t));
        newtag.key = pmt::deserialize(sb);
        newtag.value = pmt::deserialize(sb);
        newtag.srcid = pmt::deserialize(sb);
        tags_out.push_back(newtag);
    }

    return msg.size() - sb.in_avail();
}
} /* namespace zeromq */
} /* namespace gr */

// vim: ts=2 sw=2 expandtab
