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

#include "pdu_set_impl.h"
#include <gnuradio/blocks/pdu.h>
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

pdu_set::sptr pdu_set::make(pmt::pmt_t k, pmt::pmt_t v)
{
    return gnuradio::get_initial_sptr(new pdu_set_impl(k, v));
}

pdu_set_impl::pdu_set_impl(pmt::pmt_t k, pmt::pmt_t v)
    : block("pdu_set", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_k(k),
      d_v(v)
{
    message_port_register_out(pdu::pdu_port_id());
    message_port_register_in(pdu::pdu_port_id());
    set_msg_handler(pdu::pdu_port_id(), boost::bind(&pdu_set_impl::handle_msg, this, _1));
}

void pdu_set_impl::handle_msg(pmt::pmt_t pdu)
{
    // add the field and publish
    pmt::pmt_t meta = pmt::car(pdu);
    if (pmt::is_null(meta)) {
        meta = pmt::make_dict();
    } else if (!pmt::is_dict(meta)) {
        throw std::runtime_error("pdu_set received non PDU input");
    }
    meta = pmt::dict_add(meta, d_k, d_v);
    message_port_pub(pdu::pdu_port_id(), pmt::cons(meta, pmt::cdr(pdu)));
}

} /* namespace blocks */
} /* namespace gr */
