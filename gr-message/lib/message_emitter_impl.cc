/* -*- c++ -*- */
/*
 * Copyright 2020 NTESS LLC
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "message_emitter_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/message/port_names.h>

namespace gr {
namespace message {

message_emitter::sptr message_emitter::make(pmt::pmt_t msg)
{
    return gnuradio::make_block_sptr<message_emitter_impl>(msg);
}

/*
 * The private constructor
 */
message_emitter_impl::message_emitter_impl(pmt::pmt_t msg)
    : gr::block(
          "message_emitter", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_msg(msg),
      d_n_msgs(0)
{
    message_port_register_in(PMTCONSTSTR__emit());
    set_msg_handler(PMTCONSTSTR__emit(), [this](pmt::pmt_t msg) { this->emit(msg); });
    message_port_register_out(PMTCONSTSTR__msg());
}

message_emitter_impl::~message_emitter_impl() {}


/*
 * This goes here as print statements in the destructor are usually not shown
 */
bool message_emitter_impl::stop()
{
    GR_LOG_INFO(d_logger, boost::format("Message emitter sent %d messages") % d_n_msgs);

    return true;
}


void message_emitter_impl::setup_rpc()
{
#ifdef GR_CTRLPORT

    rpcbasic_sptr(new rpcbasic_register_handler<message_emitter>(
        alias(), "emit", "", "Configure settings", RPC_PRIVLVL_MIN, DISPNULL));

#endif /* GR_CTRLPORT */
}


/*
 * The only thing this block does... emit messages when emit() is called -
 * this can be the defined message or a provided one
 */
void message_emitter_impl::emit()
{
    d_n_msgs++;
    message_port_pub(PMTCONSTSTR__msg(), d_msg);
}

void message_emitter_impl::emit(pmt::pmt_t msg)
{
    gr::thread::scoped_lock l(d_setlock);

    d_n_msgs++;
    /* if the emitter argument is PMT_NIL, send the stored message*/
    if (pmt::eqv(msg, pmt::PMT_NIL)) {
        message_port_pub(PMTCONSTSTR__msg(), d_msg);
    } else {
        message_port_pub(PMTCONSTSTR__msg(), msg);
    }
}

void message_emitter_impl::set_msg(pmt::pmt_t msg)
{
    gr::thread::scoped_lock l(d_setlock);

    d_msg = msg;
}

} /* namespace message */
} /* namespace gr */
