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

#include "pdu_filter_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>

namespace gr {
namespace pdu {

pdu_filter::sptr pdu_filter::make(pmt::pmt_t k, pmt::pmt_t v, bool invert)
{
    return gnuradio::make_block_sptr<pdu_filter_impl>(k, v, invert);
}

pdu_filter_impl::pdu_filter_impl(pmt::pmt_t k, pmt::pmt_t v, bool invert)
    : block("pdu_filter", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_k(k),
      d_v(v),
      d_invert(invert)
{
    message_port_register_out(msgport_names::pdus());
    message_port_register_in(msgport_names::pdus());
    set_msg_handler(msgport_names::pdus(),
                    [this](pmt::pmt_t msg) { this->handle_msg(msg); });
}

void pdu_filter_impl::handle_msg(pmt::pmt_t pdu)
{
    pmt::pmt_t meta = pmt::car(pdu);
    bool output = d_invert;

    // check base type
    // key exists
    // value matches
    if (pmt::is_dict(meta) && dict_has_key(meta, d_k) &&
        pmt::eqv(pmt::dict_ref(meta, d_k, pmt::PMT_NIL), d_v)) {
        output = !d_invert;
    }

    // if all tests pass, propagate the pdu
    if (output) {
        message_port_pub(msgport_names::pdus(), pdu);
    }
}

} /* namespace pdu */
} /* namespace gr */
