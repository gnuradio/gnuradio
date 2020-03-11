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
#include <gnuradio/blocks/pdu.h>
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

pdu_filter::sptr pdu_filter::make(pmt::pmt_t k, pmt::pmt_t v, bool invert)
{
    return gnuradio::get_initial_sptr(new pdu_filter_impl(k, v, invert));
}

pdu_filter_impl::pdu_filter_impl(pmt::pmt_t k, pmt::pmt_t v, bool invert)
    : block("pdu_filter", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_k(k),
      d_v(v),
      d_invert(invert)
{
    message_port_register_out(pdu::pdu_port_id());
    message_port_register_in(pdu::pdu_port_id());
    set_msg_handler(pdu::pdu_port_id(),
                    boost::bind(&pdu_filter_impl::handle_msg, this, _1));
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
        message_port_pub(pdu::pdu_port_id(), pdu);
    }
}

} /* namespace blocks */
} /* namespace gr */
