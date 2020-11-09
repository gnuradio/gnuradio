/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_NOP_IMPL_H
#define INCLUDED_GR_NOP_IMPL_H

#include <gnuradio/blocks/nop.h>

namespace gr {
namespace blocks {

class nop_impl : public nop
{
protected:
    int d_nmsgs_recvd;
    int d_ctrlport_test;

    // Method that just counts any received messages.
    void count_received_msgs(pmt::pmt_t msg);

public:
    nop_impl(size_t sizeof_stream_item);
    ~nop_impl() override;

    void setup_rpc() override;

    int nmsgs_received() const override { return d_nmsgs_recvd; }

    int ctrlport_test() const override { return d_ctrlport_test; }
    void set_ctrlport_test(int x) override { d_ctrlport_test = x; }

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_NOP_IMPL_H */
