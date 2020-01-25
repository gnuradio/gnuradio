/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fading_model_impl.h"
#include <gnuradio/io_signature.h>


namespace gr {
namespace channels {

fading_model::sptr
fading_model::make(unsigned int N, float fDTs, bool LOS, float K, uint32_t seed)
{
    return gnuradio::get_initial_sptr(new fading_model_impl(N, fDTs, LOS, K, seed));
}

// Block constructor
fading_model_impl::fading_model_impl(
    unsigned int N, float fDTs, bool LOS, float K, uint32_t seed)
    : sync_block("fading_model",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_fader(N, fDTs, LOS, K, seed)
{
}

fading_model_impl::~fading_model_impl() {}

void fading_model_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_get<fading_model, float>(
        alias(),
        "fDTs",
        &fading_model::fDTs,
        pmt::mp(0),
        pmt::mp(1),
        pmt::mp(0.01),
        "Hz*Sec",
        "normalized maximum doppler frequency (fD*Ts)",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_set<fading_model, float>(
        alias(),
        "fDTs",
        &fading_model::set_fDTs,
        pmt::mp(0),
        pmt::mp(1),
        pmt::mp(0.01),
        "Hz*Sec",
        "normalized maximum doppler frequency (fD*Ts)",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_get<fading_model, float>(
        alias(),
        "K",
        &fading_model::K,
        pmt::mp(0),
        pmt::mp(8),
        pmt::mp(4),
        "Ratio",
        "Rician factor (ratio of the specular power to the scattered power)",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_set<fading_model, float>(
        alias(),
        "K",
        &fading_model::set_K,
        pmt::mp(0),
        pmt::mp(8),
        pmt::mp(4),
        "Ratio",
        "Rician factor (ratio of the specular power to the scattered power)",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_get<fading_model, float>(
        alias(),
        "step",
        &fading_model::step,
        pmt::mp(0),
        pmt::mp(8),
        pmt::mp(4),
        "radians",
        "Maximum step size for random walk angle per sample",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_set<fading_model, float>(
        alias(),
        "step",
        &fading_model::set_step,
        pmt::mp(0),
        pmt::mp(1),
        pmt::mp(0.00001),
        "radians",
        "Maximum step size for random walk angle per sample",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
}

int fading_model_impl::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];
    std::vector<gr_complex> ftaps;
    d_fader.next_samples(ftaps, noutput_items);
    for (int i = 0; i < noutput_items; i++) {
        out[i] = in[i] * ftaps[i];
        // out[i] = in[i] * d_fader.next_sample();
    }
    return noutput_items;
}


} /* namespace channels */
} /* namespace gr */
