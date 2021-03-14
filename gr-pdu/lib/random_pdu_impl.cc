/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "random_pdu_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>

namespace gr {
namespace pdu {

random_pdu::sptr
random_pdu::make(int min_items, int max_items, unsigned char byte_mask, int length_modulo)
{
    return gnuradio::make_block_sptr<random_pdu_impl>(
        min_items, max_items, byte_mask, length_modulo);
}

random_pdu_impl::random_pdu_impl(int min_items,
                                 int max_items,
                                 unsigned char byte_mask,
                                 int length_modulo)
    : block("random_pdu", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_urange(min_items, max_items),
      d_brange(0, 255),
      d_mask(byte_mask),
      d_length_modulo(length_modulo)
{
    message_port_register_out(msgport_names::pdus());
    message_port_register_in(pmt::mp("generate"));
    set_msg_handler(pmt::mp("generate"),
                    [this](pmt::pmt_t msg) { this->generate_pdu(msg); });
    if (length_modulo < 1)
        throw std::runtime_error("length_module must be >= 1");
    if (max_items < length_modulo)
        throw std::runtime_error("max_items must be >= to length_modulo");
}

bool random_pdu_impl::start()
{
    output_random();
    return true;
}

void random_pdu_impl::output_random()
{
    // pick a random vector length
    int len = d_urange(d_rng);
    len = std::max(d_length_modulo, len - len % d_length_modulo);

    // fill it with random bytes
    std::vector<unsigned char> vec(len);
    for (int i = 0; i < len; i++)
        vec[i] = ((unsigned char)d_brange(d_rng)) & d_mask;

    // send the vector
    pmt::pmt_t vecpmt(pmt::make_blob(&vec[0], len));
    pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL, vecpmt));

    message_port_pub(msgport_names::pdus(), pdu);
}

} /* namespace pdu */
} /* namespace gr */
