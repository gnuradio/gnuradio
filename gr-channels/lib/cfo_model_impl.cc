/* -*- c++ -*- */
/*
 * Copyright 2009,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "cfo_model_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <iostream>

namespace gr {
namespace channels {

cfo_model::sptr cfo_model::make(double sample_rate_hz,
                                double std_dev_hz,
                                double max_dev_hz,
                                double noise_seed)
{
    return gnuradio::make_block_sptr<cfo_model_impl>(
        sample_rate_hz, std_dev_hz, max_dev_hz, noise_seed);
}

cfo_model_impl::cfo_model_impl(double sample_rate_hz,
                               double std_dev_hz,
                               double max_dev_hz,
                               double noise_seed)
    : sync_block("cfo_model",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_samp_rate(sample_rate_hz),
      d_max_dev_hz(max_dev_hz),
      d_table(8 * 1024),
      d_noise(gr::analog::fastnoise_source_f::make(
          analog::GR_GAUSSIAN, std_dev_hz, noise_seed)),
      d_cfo(0),
      d_angle(0),
      d_noise_seed(noise_seed)
{
}

cfo_model_impl::~cfo_model_impl() {}

int cfo_model_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{

    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];
    for (int i = 0; i < noutput_items; i++) {
        // update and bound cfo
        // we multiply by a random {1,-1} to remove any sign
        // bias that may exist in our random sample pool
        d_cfo += d_noise->sample_unbiased();
        d_cfo = std::min(d_cfo, d_max_dev_hz);
        d_cfo = std::max(d_cfo, -d_max_dev_hz);
        // update and wrap angle
        d_angle += 2 * GR_M_PI * d_cfo / d_samp_rate;
        d_angle = d_angle > 2 * GR_M_PI ? d_angle - 2 * GR_M_PI : d_angle;
        d_angle = d_angle < -2 * GR_M_PI ? d_angle + 2 * GR_M_PI : d_angle;
        out[i] = in[i] * gr_complex(d_table.cos(d_angle), d_table.sin(d_angle));
    }
    return noutput_items;
}

void cfo_model_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_variable<double>(alias(),
                                                             "cfo",
                                                             &d_cfo,
                                                             pmt::mp(-10.0f),
                                                             pmt::mp(10.0f),
                                                             pmt::mp(0.0f),
                                                             "",
                                                             "Current CFO in Hz",
                                                             RPC_PRIVLVL_MIN,
                                                             DISPTIME | DISPOPTSTRIP)));
    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_variable<double>(alias(),
                                                             "cfo_max",
                                                             &d_max_dev_hz,
                                                             pmt::mp(-10.0f),
                                                             pmt::mp(10.0f),
                                                             pmt::mp(0.0f),
                                                             "",
                                                             "Max CFO in Hz",
                                                             RPC_PRIVLVL_MIN,
                                                             DISPTIME | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
}

} /* namespace channels */
} /* namespace gr */
