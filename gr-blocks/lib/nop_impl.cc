/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nop_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

nop::sptr nop::make(size_t sizeof_stream_item)
{
    return gnuradio::make_block_sptr<nop_impl>(sizeof_stream_item);
}

nop_impl::nop_impl(size_t sizeof_stream_item)
    : block("nop",
            io_signature::make(0, -1, sizeof_stream_item),
            io_signature::make(0, -1, sizeof_stream_item)),
      d_nmsgs_recvd(0)
{
    // Arrange to have count_received_msgs called when messages are received.
    message_port_register_in(pmt::mp("port"));
    set_msg_handler(pmt::mp("port"),
                    [this](pmt::pmt_t msg) { this->count_received_msgs(msg); });
}

nop_impl::~nop_impl() {}

// Trivial message handler that just counts them.
// (N.B., This feature is used in qa_set_msg_handler)
void nop_impl::count_received_msgs(pmt::pmt_t msg) { d_nmsgs_recvd++; }

int nop_impl::general_work(int noutput_items,
                           gr_vector_int& ninput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    // eat any input that's available
    for (unsigned i = 0; i < ninput_items.size(); i++)
        consume(i, ninput_items[i]);

    return noutput_items;
}

void nop_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    d_rpc_vars.emplace_back(new rpcbasic_register_get<nop, int>(alias(),
                                                                "test",
                                                                &nop::ctrlport_test,
                                                                pmt::mp(-256),
                                                                pmt::mp(255),
                                                                pmt::mp(0),
                                                                "",
                                                                "Simple testing variable",
                                                                RPC_PRIVLVL_MIN,
                                                                DISPNULL));

    d_rpc_vars.emplace_back(
        rpcbasic_sptr(new rpcbasic_register_set<nop, int>(alias(),
                                                          "test",
                                                          &nop::set_ctrlport_test,
                                                          pmt::mp(-256),
                                                          pmt::mp(255),
                                                          pmt::mp(0),
                                                          "",
                                                          "Simple testing variable",
                                                          RPC_PRIVLVL_MIN,
                                                          DISPNULL)));
#endif /* GR_CTRLPORT */
}

} /* namespace blocks */
} /* namespace gr */
