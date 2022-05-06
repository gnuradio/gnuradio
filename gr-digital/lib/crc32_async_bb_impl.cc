/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "crc32_async_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace digital {

crc32_async_bb::sptr crc32_async_bb::make(bool check)
{
    return gnuradio::make_block_sptr<crc32_async_bb_impl>(check);
}

crc32_async_bb_impl::crc32_async_bb_impl(bool check)
    : block("crc32_async_bb", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_crc_impl(32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true),
      d_npass(0),
      d_nfail(0)
{
    d_in_port = pmt::mp("in");
    d_out_port = pmt::mp("out");

    message_port_register_in(d_in_port);
    message_port_register_out(d_out_port);

    if (check)
        set_msg_handler(d_in_port, [this](pmt::pmt_t msg) { this->check(msg); });
    else
        set_msg_handler(d_in_port, [this](pmt::pmt_t msg) { this->calc(msg); });
}

crc32_async_bb_impl::~crc32_async_bb_impl() {}

void crc32_async_bb_impl::calc(pmt::pmt_t msg)
{
    // extract input pdu
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bytes(pmt::cdr(msg));

    unsigned int crc;
    size_t pkt_len(0);
    const uint8_t* bytes_in = pmt::u8vector_elements(bytes, pkt_len);
    std::vector<uint8_t> bytes_out(4 + pkt_len);

    crc = d_crc_impl.compute(bytes_in, pkt_len);
    memcpy((void*)bytes_out.data(), (const void*)bytes_in, pkt_len);
    memcpy((void*)(bytes_out.data() + pkt_len),
           &crc,
           4); // FIXME big-endian/little-endian, this might be wrong

    pmt::pmt_t output = pmt::init_u8vector(
        pkt_len + 4,
        bytes_out.data()); // this copies the values from bytes_out into the u8vector
    pmt::pmt_t msg_pair = pmt::cons(meta, output);
    message_port_pub(d_out_port, msg_pair);
}

void crc32_async_bb_impl::check(pmt::pmt_t msg)
{
    // extract input pdu
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bytes(pmt::cdr(msg));

    unsigned int crc;
    size_t pkt_len(0);
    const uint8_t* bytes_in = pmt::u8vector_elements(bytes, pkt_len);

    crc = d_crc_impl.compute(bytes_in, pkt_len - 4);
    if (memcmp(&crc, bytes_in + pkt_len - 4, 4)) { // Drop package
        d_nfail++;
        return;
    }
    d_npass++;

    pmt::pmt_t output = pmt::init_u8vector(pkt_len - 4, bytes_in);
    pmt::pmt_t msg_pair = pmt::cons(meta, output);
    message_port_pub(d_out_port, msg_pair);
}

int crc32_async_bb_impl::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
