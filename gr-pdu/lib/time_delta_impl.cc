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

#include "time_delta_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>
#include <chrono>

namespace gr {
namespace pdu {

time_delta::sptr time_delta::make(const pmt::pmt_t delta_key, const pmt::pmt_t time_key)
{
    return gnuradio::make_block_sptr<time_delta_impl>(delta_key, time_key);
}

time_delta_impl::time_delta_impl(const pmt::pmt_t delta_key, const pmt::pmt_t time_key)
    : gr::block("time_delta", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_name(pmt::symbol_to_string(delta_key)),
      d_delta_key(delta_key),
      d_time_key(time_key),
      d_mean(0.0),
      d_var_acc(0.0),
      d_var(0.0),
      d_n(0)
{
    message_port_register_in(msgport_names::pdu());
    set_msg_handler(msgport_names::pdu(),
                    [this](const pmt::pmt_t& msg) { this->handle_pdu(msg); });
    message_port_register_out(msgport_names::pdu());
}

time_delta_impl::~time_delta_impl() {}

bool time_delta_impl::stop()
{
    d_logger->notice("Statistics for {:s}: Mean = {:.6f} ms, Var = {:.6f} ms",
                     d_name,
                     d_mean,
                     get_variance());
    return true;
}

void time_delta_impl::reset_stats()
{
    // do not allow statistics to be reset while they are being accessed
    gr::thread::scoped_lock l(d_mutex);

    d_n = 0;
    d_mean = 0;
    d_var_acc = 0;
    d_var = 0;
}

void time_delta_impl::update_stats(double x)
{
    // uses Welford's Method to calculate the running variance
    double prev_mean = d_mean;
    d_n++;
    d_mean = d_mean + (x - d_mean) / d_n;
    d_var_acc += (x - d_mean) * (x - prev_mean);
    d_var = d_var_acc / (d_n - 1);
}

void time_delta_impl::handle_pdu(const pmt::pmt_t& pdu)
{
    // make sure the message is a PDU
    if (!(pmt::is_pdu(pdu))) {
        d_logger->warn("Message received is not a PDU, dropping");
        return;
    }

    gr::thread::scoped_lock l(d_mutex);

    double t_now(
        std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch())
            .count());

    pmt::pmt_t meta = pmt::car(pdu);
    pmt::pmt_t sys_time_pmt = pmt::dict_ref(meta, d_time_key, pmt::PMT_NIL);
    if (!pmt::is_real(sys_time_pmt)) {
        d_logger->info("PDU received with no system time at {:f}", t_now);
        return;
    }

    double pdu_time = pmt::to_double(sys_time_pmt);
    double time_delta_ms = (t_now - pdu_time) * 1000.0;
    d_logger->debug("{:s} PDU received at {:f} with time delta {:f} milliseconds",
                    d_name,
                    t_now,
                    time_delta_ms);
    update_stats(time_delta_ms);

    meta = pmt::dict_add(meta, d_delta_key, pmt::from_double(time_delta_ms));
    message_port_pub(msgport_names::pdu(), pmt::cons(meta, pmt::cdr(pdu)));
}

} /* namespace pdu */
} /* namespace gr */
