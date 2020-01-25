/* -*- c++ -*- */
/*
 * Copyright 2015-2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "protocol_parser_b_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <iostream>
#include <stdexcept>

namespace gr {
namespace digital {

protocol_parser_b::sptr protocol_parser_b::make(const header_format_base::sptr& format)
{
    return gnuradio::get_initial_sptr(new protocol_parser_b_impl(format));
}


protocol_parser_b_impl::protocol_parser_b_impl(const header_format_base::sptr& format)
    : sync_block("protocol_parser_b",
                 io_signature::make(1, 1, sizeof(char)),
                 io_signature::make(0, 0, 0))
{
    d_format = format;

    d_out_port = pmt::mp("info");
    message_port_register_out(d_out_port);
}

protocol_parser_b_impl::~protocol_parser_b_impl() {}

int protocol_parser_b_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];

    int count = 0;
    std::vector<pmt::pmt_t> info;
    bool ret = d_format->parse(noutput_items, in, info, count);

    if (ret) {
        for (size_t i = 0; i < info.size(); i++) {
            message_port_pub(d_out_port, info[i]);
        }
    } else {
        message_port_pub(d_out_port, pmt::PMT_F);
    }

    return count;
}

} /* namespace digital */
} /* namespace gr */
