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

#include "message_counter_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/message/port_names.h>

namespace gr {
namespace message {

message_counter::sptr message_counter::make(std::string name)
{
    return gnuradio::make_block_sptr<message_counter_impl>(name);
}

/**
 * Constructor
 *
 * @param name - name of counter
 */
message_counter_impl::message_counter_impl(std::string name)
    : gr::block(
          "message_counter", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_name(name),
      d_ctr(0)
{
    message_port_register_in(PMTCONSTSTR__msg());
    set_msg_handler(PMTCONSTSTR__msg(),
                    [this](pmt::pmt_t msg) { this->handle_msg(msg); });
}

message_counter_impl::~message_counter_impl() {}


/*
 * This goes here as print statements in the destructor are usually not shown
 */
bool message_counter_impl::stop()
{
#ifdef ENABLE_GR_LOG
    GR_LOG_INFO(d_logger,
                boost::format("Message Counter \"'%s'\" got %d messages") % d_name %
                    d_ctr);
#else
    std::cout << alias() << " :INFO: Message counter " << d_name << " got " << d_ctr
              << " messages" << std::endl;
#endif

    return true;
}


void message_counter_impl::setup_rpc()
{
#ifdef GR_CTRLPORT

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<message_counter, uint64_t>(alias(),
                                                             "cp_get_counter",
                                                             &message_counter::get_ctr,
                                                             pmt::mp(0),
                                                             pmt::mp(10000),
                                                             pmt::mp(100),
                                                             "",
                                                             "Get counter",
                                                             RPC_PRIVLVL_MIN,
                                                             DISPTIME | DISPOPTSTRIP)));

#endif /* GR_CTRLPORT */
}

void message_counter_impl::handle_msg(pmt::pmt_t msg) { d_ctr++; }

} /* namespace message */
} /* namespace gr */
