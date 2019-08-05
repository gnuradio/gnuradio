/* -*- c++ -*- */
/*
 * Copyright 2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "selective_fading_model2_impl.h"
#include "sincostable.h"

#include <gnuradio/fxpt.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

#include <boost/format.hpp>
#include <boost/random.hpp>

#include <iostream>


// FASTSINCOS:  0 = slow native,  1 = gr::fxpt impl,  2 = sincostable.h
#define FASTSINCOS 2


namespace gr {
namespace channels {

selective_fading_model2::sptr
selective_fading_model2::make(unsigned int N,
                              float fDTs,
                              bool LOS,
                              float K,
                              int seed,
                              std::vector<float> delays,
                              std::vector<float> delays_std,
                              std::vector<float> delays_maxdev,
                              std::vector<float> mags,
                              int ntaps)
{
    return gnuradio::get_initial_sptr(new selective_fading_model2_impl(
        N, fDTs, LOS, K, seed, delays, delays_std, delays_maxdev, mags, ntaps));
}

// Block constructor
selective_fading_model2_impl::selective_fading_model2_impl(
    unsigned int N,
    float fDTs,
    bool LOS,
    float K,
    int seed,
    std::vector<float> delays,
    std::vector<float> delays_std,
    std::vector<float> delays_maxdev,
    std::vector<float> mags,
    int ntaps)
    : sync_block("selective_fading_model2",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_delays(delays),
      d_delays_orig(delays),
      d_delays_std(delays_std),
      d_delays_maxdev(delays_maxdev),
      d_mags(mags),
      d_sintable(1024),
      seed_1(0),
      dist_1(0, 1),
      rv_1(seed_1, dist_1)
{
    if (mags.size() != delays.size())
        throw std::runtime_error("magnitude and delay vectors must be the same length!");
    if (mags.size() != delays_std.size())
        throw std::runtime_error("delay std dev vector length must be the same length!");
    if (mags.size() != delays_maxdev.size())
        throw std::runtime_error("delay maxdev vector length must be the same length!");

    for (size_t i = 0; i < mags.size(); i++) {
        d_faders.push_back(
            new gr::channels::flat_fader_impl(N, fDTs, (i == 0) && (LOS), K, seed + i));
    }

    // set up tap history
    if (ntaps < 1) {
        throw std::runtime_error("ntaps must be >= 1");
    }
    set_history(ntaps);
    d_taps.resize(ntaps, gr_complex(0, 0));

    // set up message port
    message_port_register_out(pmt::mp("taps"));
}

selective_fading_model2_impl::~selective_fading_model2_impl()
{
    for (size_t i = 0; i < d_faders.size(); i++) {
        delete d_faders[i];
    }
}

int selective_fading_model2_impl::work(int noutput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    // pregenerate fading components
    std::vector<std::vector<gr_complex>> fading_taps;
    for (size_t j = 0; j < d_faders.size(); j++) {
        fading_taps.push_back(std::vector<gr_complex>());
        d_faders[j]->next_samples(fading_taps[j], noutput_items);
    }

    // loop over each output sample
    for (int i = 0; i < noutput_items; i++) {

        // move the tap delays around (random walk + clipping)
        for (size_t j = 0; j < d_faders.size(); j++) {
            float tmp = d_delays[j] + rv_1() * d_delays_std[j];
            d_delays[j] = std::max(std::min(tmp, d_delays_orig[j] + d_delays_maxdev[j]),
                                   d_delays_orig[j] - d_delays_maxdev[j]);
        }

        // clear the current values in each tap
        for (size_t j = 0; j < d_taps.size(); j++) {
            d_taps[j] = gr_complex(0, 0);
        }

        // add each flat fading component to the taps
        for (size_t j = 0; j < d_faders.size(); j++) {
            gr_complex ff_H(fading_taps[j][i]);
            // gr_complex ff_H(d_faders[j]->next_sample());
            for (size_t k = 0; k < d_taps.size(); k++) {
                float dist = k - d_delays[j];
                float interpmag = d_sintable.sinc(2 * GR_M_PI * dist);
                d_taps[k] += ff_H * interpmag * d_mags[j];
            }
        }

        // apply the taps and generate output
        gr_complex sum(0, 0);
        for (size_t j = 0; j < d_taps.size(); j++) {
            sum += in[i + j] * d_taps[d_taps.size() - j - 1];
        }

        // assign output
        out[i] = sum;
    }

    if (pmt::length(message_subscribers(pmt::mp("taps"))) > 0) {
        pmt::pmt_t pdu(
            pmt::cons(pmt::PMT_NIL, pmt::init_c32vector(d_taps.size(), d_taps)));
        message_port_pub(pmt::mp("taps"), pdu);
    }

    // return all outputs
    return noutput_items;
}

void selective_fading_model2_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<selective_fading_model2, float>(
            alias(),
            "fDTs",
            &selective_fading_model2::fDTs,
            pmt::mp(0),
            pmt::mp(1),
            pmt::mp(0.01),
            "Hz*Sec",
            "normalized maximum doppler frequency (fD*Ts)",
            RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));
    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<selective_fading_model2, float>(
            alias(),
            "fDTs",
            &selective_fading_model2::set_fDTs,
            pmt::mp(0),
            pmt::mp(1),
            pmt::mp(0.01),
            "Hz*Sec",
            "normalized maximum doppler frequency (fD*Ts)",
            RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<selective_fading_model2, float>(
            alias(),
            "K",
            &selective_fading_model2::K,
            pmt::mp(0),
            pmt::mp(8),
            pmt::mp(4),
            "Ratio",
            "Rician factor (ratio of the specular power to the scattered power)",
            RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));
    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<selective_fading_model2, float>(
            alias(),
            "K",
            &selective_fading_model2::set_K,
            pmt::mp(0),
            pmt::mp(8),
            pmt::mp(4),
            "Ratio",
            "Rician factor (ratio of the specular power to the scattered power)",
            RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<selective_fading_model2, float>(
            alias(),
            "step",
            &selective_fading_model2::step,
            pmt::mp(0),
            pmt::mp(8),
            pmt::mp(4),
            "radians",
            "Maximum step size for random walk angle per sample",
            RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));
    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<selective_fading_model2, float>(
            alias(),
            "step",
            &selective_fading_model2::set_step,
            pmt::mp(0),
            pmt::mp(1),
            pmt::mp(0.00001),
            "radians",
            "Maximum step size for random walk angle per sample",
            RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
}

} /* namespace channels */
} /* namespace gr */
