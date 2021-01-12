/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "probe_mpsk_snr_est_c_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <memory>

namespace gr {
namespace digital {

namespace {
std::unique_ptr<mpsk_snr_est> choose_type(snr_est_type_t t, double alpha)
{
    switch (t) {
    case (SNR_EST_SIMPLE):
        return std::make_unique<mpsk_snr_est_simple>(alpha);
    case (SNR_EST_SKEW):
        return std::make_unique<mpsk_snr_est_skew>(alpha);
    case (SNR_EST_M2M4):
        return std::make_unique<mpsk_snr_est_m2m4>(alpha);
    case (SNR_EST_SVR):
        return std::make_unique<mpsk_snr_est_svr>(alpha);
    }
    throw std::invalid_argument("probe_mpsk_snr_est_c_impl: unknown type specified.");
}
} // namespace

probe_mpsk_snr_est_c::sptr
probe_mpsk_snr_est_c::make(snr_est_type_t type, int msg_nsamples, double alpha)
{
    return gnuradio::make_block_sptr<probe_mpsk_snr_est_c_impl>(
        type, msg_nsamples, alpha);
}

probe_mpsk_snr_est_c_impl::probe_mpsk_snr_est_c_impl(snr_est_type_t type,
                                                     int msg_nsamples,
                                                     double alpha)
    : sync_block("probe_mpsk_snr_est_c",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(0, 0, 0)),
      d_type(type),
      d_nsamples(msg_nsamples),
      d_count(0)
{
    set_alpha(alpha);

    set_type(type);

    // at least 1 estimator has to look back
    set_history(2);

    d_snr_port = pmt::string_to_symbol("snr");
    d_signal_port = pmt::string_to_symbol("signal");
    d_noise_port = pmt::string_to_symbol("noise");

    message_port_register_out(d_snr_port);
    message_port_register_out(d_signal_port);
    message_port_register_out(d_noise_port);
}

probe_mpsk_snr_est_c_impl::~probe_mpsk_snr_est_c_impl() {}

int probe_mpsk_snr_est_c_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    int n = d_snr_est->update(noutput_items, in);

    d_count += noutput_items;
    while (d_count > d_nsamples) {
        // Post a message with the latest SNR data
        message_port_pub(d_snr_port, pmt::from_double(snr()));
        message_port_pub(d_signal_port, pmt::from_double(signal()));
        message_port_pub(d_noise_port, pmt::from_double(noise()));
        d_count -= d_nsamples;
    }

    return n;
}

double probe_mpsk_snr_est_c_impl::snr()
{
    if (d_snr_est)
        return d_snr_est->snr();
    else
        throw std::runtime_error("probe_mpsk_snr_est_c_impl:: No SNR estimator defined.");
}

double probe_mpsk_snr_est_c_impl::signal()
{
    if (d_snr_est)
        return d_snr_est->signal();
    else
        throw std::runtime_error("probe_mpsk_snr_est_c_impl:: No SNR estimator defined.");
}


double probe_mpsk_snr_est_c_impl::noise()
{
    if (d_snr_est)
        return d_snr_est->noise();
    else
        throw std::runtime_error("probe_mpsk_snr_est_c_impl:: No SNR estimator defined.");
}

snr_est_type_t probe_mpsk_snr_est_c_impl::type() const { return d_type; }

int probe_mpsk_snr_est_c_impl::msg_nsample() const { return d_nsamples; }

double probe_mpsk_snr_est_c_impl::alpha() const { return d_alpha; }

void probe_mpsk_snr_est_c_impl::set_type(snr_est_type_t t)
{
    d_snr_est = choose_type(t, d_alpha);
    d_type = t;
}

void probe_mpsk_snr_est_c_impl::set_msg_nsample(int n)
{
    if (n > 0) {
        d_nsamples = n;
        d_count = 0; // reset state
    } else
        throw std::invalid_argument(
            "probe_mpsk_snr_est_c_impl: msg_nsamples can't be <= 0");
}

void probe_mpsk_snr_est_c_impl::set_alpha(double alpha)
{
    if ((alpha >= 0) && (alpha <= 1.0)) {
        d_alpha = alpha;
        if (d_snr_est)
            d_snr_est->set_alpha(d_alpha);
    } else
        throw std::invalid_argument("probe_mpsk_snr_est_c_impl: alpha must be in [0,1]");
}

} /* namespace digital */
} /* namespace gr */
