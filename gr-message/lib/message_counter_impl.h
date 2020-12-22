/* -*- c++ -*- */
/*
 * Copyright 2020 NTESS LLC
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MESSAGE_MESSAGE_COUNTER_IMPL_H
#define INCLUDED_MESSAGE_MESSAGE_COUNTER_IMPL_H

#include <gnuradio/message/message_counter.h>
#include <pmt/pmt.h>

namespace gr {
namespace message {
/**
 * Implementing class of message_counter
 */
class message_counter_impl : public message_counter
{
private:
    std::string d_name;
    uint64_t d_ctr;

public:
    message_counter_impl(std::string name);

    ~message_counter_impl();
    virtual bool stop(); // overloaded for print output
    void setup_rpc();    // enable controlport

    void handle_msg(pmt::pmt_t msg);

    void reset(void) { d_ctr = 0; }

    uint64_t get_ctr() { return d_ctr; }

    std::string get_name(void) { return d_name; }
};

} // namespace message
} // namespace gr

#endif /* INCLUDED_MESSAGE_MESSAGE_COUNTER_IMPL_H */
