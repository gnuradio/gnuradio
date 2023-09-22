/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pdu_split_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>

namespace gr {
namespace pdu {

pdu_split::sptr pdu_split::make(const bool pass_empty_data)
{
    return gnuradio::make_block_sptr<pdu_split_impl>(pass_empty_data);
}

pdu_split_impl::pdu_split_impl(const bool pass_empty_data)
    : gr::block("pdu_split", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_pass_empty_data(pass_empty_data)
{
    message_port_register_in(msgport_names::pdu());
    set_msg_handler(msgport_names::pdu(),
                    [this](const pmt::pmt_t& msg) { this->handle_pdu(msg); });
    message_port_register_out(msgport_names::dict());
    message_port_register_out(msgport_names::vec());
}

pdu_split_impl::~pdu_split_impl() {}

void pdu_split_impl::handle_pdu(const pmt::pmt_t& pdu)
{
    // make sure the message is a PDU
    if (!(pmt::is_pdu(pdu))) {
        d_logger->warn("Message received is not a PDU, dropping");
        return;
    }

    pmt::pmt_t meta = pmt::car(pdu);
    pmt::pmt_t data = pmt::cdr(pdu);

    if ((!pmt::equal(meta, pmt::PMT_NIL)) | d_pass_empty_data) {
        message_port_pub(msgport_names::dict(), meta);
    }

    if (pmt::length(data) | d_pass_empty_data) {
        message_port_pub(msgport_names::vec(), data);
    }
}

} /* namespace pdu */
} /* namespace gr */
