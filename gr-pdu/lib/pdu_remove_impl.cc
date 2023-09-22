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

#include "pdu_remove_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>

namespace gr {
namespace pdu {

pdu_remove::sptr pdu_remove::make(pmt::pmt_t k)
{
    return gnuradio::make_block_sptr<pdu_remove_impl>(k);
}

pdu_remove_impl::pdu_remove_impl(pmt::pmt_t k)
    : block("pdu_remove", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_k(k)
{
    message_port_register_out(msgport_names::pdus());
    message_port_register_in(msgport_names::pdus());
    set_msg_handler(msgport_names::pdus(),
                    [this](pmt::pmt_t msg) { this->handle_msg(msg); });
}

void pdu_remove_impl::handle_msg(pmt::pmt_t pdu)
{
    // add the field and publish
    pmt::pmt_t meta = pmt::car(pdu);
    if (pmt::is_null(meta)) {
        meta = pmt::make_dict();
    } else if (!pmt::is_dict(meta)) {
        throw std::runtime_error("pdu_remove received non PDU input");
    }
    meta = pmt::dict_delete(meta, d_k);
    message_port_pub(msgport_names::pdus(), pmt::cons(meta, pmt::cdr(pdu)));
}

} /* namespace pdu */
} /* namespace gr */
