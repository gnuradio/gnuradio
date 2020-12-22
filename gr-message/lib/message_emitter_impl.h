/* -*- c++ -*- */
/*
 * Copyright 2020 NTESS LLC
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MESSAGE_MESSAGE_EMITTER_IMPL_H
#define INCLUDED_MESSAGE_MESSAGE_EMITTER_IMPL_H

#include <gnuradio/message/message_emitter.h>
#include <pmt/pmt.h>

namespace gr {
namespace message {

class message_emitter_impl : public message_emitter
{
private:
    pmt::pmt_t d_msg;
    uint64_t d_n_msgs;

public:
    message_emitter_impl(pmt::pmt_t msg = pmt::PMT_NIL);

    ~message_emitter_impl();
    bool stop();      // overloaded for print output
    void setup_rpc(); // enable controlport

    void set_msg(pmt::pmt_t msg);

    uint64_t get_n_msgs(void) { return d_n_msgs; }

    void emit();

    void emit(pmt::pmt_t msg);
};

} // namespace message
} // namespace gr

#endif /* INCLUDED_MESSAGE_MESSAGE_EMITTER_IMPL_H */
