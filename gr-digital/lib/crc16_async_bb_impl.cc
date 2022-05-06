/* -*- c++ -*- */
/*
 * Copyright 2021 Cesar Martinez.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "crc16_async_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace digital {

crc16_async_bb::sptr crc16_async_bb::make(bool check)
{
    return gnuradio::make_block_sptr<crc16_async_bb_impl>(check);
}

crc16_async_bb_impl::crc16_async_bb_impl(bool check)
    : block("crc16_async_bb", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_crc_ccitt_impl(16, 0x1021, 0xFFFF, 0, false, false),
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

crc16_async_bb_impl::~crc16_async_bb_impl() {}

void crc16_async_bb_impl::calc(pmt::pmt_t msg)
{
    // extract input pdu
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bytes(pmt::cdr(msg));

    unsigned int crc;
    size_t pkt_len(0);
    const uint8_t* bytes_in = pmt::u8vector_elements(bytes, pkt_len);
    std::vector<uint8_t> bytes_out(2 + pkt_len);

    crc = d_crc_ccitt_impl.compute(bytes_in, pkt_len);
    memcpy((void*)bytes_out.data(), (const void*)bytes_in, pkt_len);
    memcpy((void*)(bytes_out.data() + pkt_len), &crc, 2);

    pmt::pmt_t output = pmt::init_u8vector(
        pkt_len + 2,
        bytes_out.data()); // this copies the values from bytes_out into the u8vector
    pmt::pmt_t msg_pair = pmt::cons(meta, output);
    message_port_pub(d_out_port, msg_pair);
}

void crc16_async_bb_impl::check(pmt::pmt_t msg)
{
    // extract input pdu
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bytes(pmt::cdr(msg));

    unsigned int crc;
    size_t pkt_len(0);
    const uint8_t* bytes_in = pmt::u8vector_elements(bytes, pkt_len);

    crc = d_crc_ccitt_impl.compute(bytes_in, pkt_len - 2);
    if (memcmp(&crc, bytes_in + pkt_len - 2, 2)) { // Drop package
        d_nfail++;
        return;
    }
    d_npass++;

    pmt::pmt_t output = pmt::init_u8vector(pkt_len - 2, bytes_in);
    pmt::pmt_t msg_pair = pmt::cons(meta, output);
    message_port_pub(d_out_port, msg_pair);
}

int crc16_async_bb_impl::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
