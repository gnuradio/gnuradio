/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "add_system_time_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>

namespace gr {
namespace pdu {

add_system_time::sptr add_system_time::make(const pmt::pmt_t key)
{
    return gnuradio::make_block_sptr<add_system_time_impl>(key);
}

/*
 * The private constructor
 */
add_system_time_impl::add_system_time_impl(const pmt::pmt_t key)
    : gr::block("add_system_time",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_name(pmt::symbol_to_string(key)),
      d_epoch(boost::gregorian::date(1970, 1, 1)),
      d_key(key)
{
    // boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    // d_epoch = epoch;

    message_port_register_in(msgport_names::pdu());
    set_msg_handler(msgport_names::pdu(),
                    [this](const pmt::pmt_t& msg) { this->handle_pdu(msg); });
    message_port_register_out(msgport_names::pdu());
}

/*
 * Our virtual destructor.
 */
add_system_time_impl::~add_system_time_impl() {}

void add_system_time_impl::handle_pdu(const pmt::pmt_t& pdu)
{
    // make sure the message is a PDU
    if (!(pmt::is_pdu(pdu))) {
        GR_LOG_WARN(d_logger, "Message received is not a PDU, dropping");
        return;
    }

    pmt::pmt_t meta = pmt::car(pdu);

    // append time and publish
    double t_now((boost::get_system_time() - d_epoch).total_microseconds() / 1000000.0);
    meta = pmt::dict_add(meta, d_key, pmt::from_double(t_now));
    message_port_pub(msgport_names::pdu(), pmt::cons(meta, pmt::cdr(pdu)));
}

} /* namespace pdu */
} /* namespace gr */
