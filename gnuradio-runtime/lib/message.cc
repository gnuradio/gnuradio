/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/message.h>
#include <cassert>
#include <cstring>

namespace gr {

static long s_ncurrently_allocated = 0;

message::sptr message::make(long type, double arg1, double arg2, size_t length)
{
    return message::sptr(new message(type, arg1, arg2, length));
}

message::sptr
message::make_from_string(const std::string s, long type, double arg1, double arg2)
{
    message::sptr m = message::make(type, arg1, arg2, s.size());
    memcpy(m->msg(), s.data(), s.size());
    return m;
}

message::message(long type, double arg1, double arg2, size_t length)
    : d_type(type), d_arg1(arg1), d_arg2(arg2), d_buf(length)
{
    if (length == 0)
        d_msg_start = d_msg_end = nullptr;
    else {
        d_msg_start = d_buf.data();
        d_msg_end = d_msg_start + length;
    }
    s_ncurrently_allocated++;
}

message::~message()
{
    assert(d_next == 0);
    s_ncurrently_allocated--;
}

std::string message::to_string() const
{
    return std::string((char*)d_msg_start, length());
}

long message_ncurrently_allocated() { return s_ncurrently_allocated; }

} /* namespace gr */
