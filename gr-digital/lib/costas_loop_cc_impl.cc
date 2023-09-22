/* -*- c++ -*- */
/*
 * Copyright 2006,2010-2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "costas_loop_cc_impl.h"
#include <gnuradio/expj.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <gnuradio/sincos.h>

namespace gr {
namespace digital {

costas_loop_cc::sptr costas_loop_cc::make(float loop_bw, unsigned int order, bool use_snr)
{
    return gnuradio::make_block_sptr<costas_loop_cc_impl>(loop_bw, order, use_snr);
}

static int ios[] = { sizeof(gr_complex), sizeof(float), sizeof(float), sizeof(float) };
static std::vector<int> iosig(ios, ios + sizeof(ios) / sizeof(int));

costas_loop_cc_impl::costas_loop_cc_impl(float loop_bw, unsigned int order, bool use_snr)
    : sync_block("costas_loop_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::makev(1, 4, iosig)),
      blocks::control_loop(loop_bw, 1.0, -1.0),
      d_error(0),
      d_noise(1.0),
      d_use_snr(use_snr),
      d_order(order)
{
    message_port_register_in(pmt::mp("noise"));
    set_msg_handler(pmt::mp("noise"),
                    [this](pmt::pmt_t msg) { this->handle_set_noise(msg); });
}

costas_loop_cc_impl::~costas_loop_cc_impl() {}

void costas_loop_cc_impl::handle_set_noise(pmt::pmt_t msg)
{
    if (pmt::is_real(msg)) {
        d_noise = pmt::to_double(msg);
        d_noise = powf(10.0f, d_noise / 10.0f);
    }
}

int costas_loop_cc_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const gr_complex* iptr = (gr_complex*)input_items[0];
    gr_complex* optr = (gr_complex*)output_items[0];
    float* freq_optr = output_items.size() >= 2 ? (float*)output_items[1] : NULL;
    float* phase_optr = output_items.size() >= 3 ? (float*)output_items[2] : NULL;
    float* error_optr = output_items.size() >= 4 ? (float*)output_items[3] : NULL;

    std::vector<tag_t> tags;
    get_tags_in_range(tags,
                      0,
                      nitems_read(0),
                      nitems_read(0) + noutput_items,
                      pmt::intern("phase_est"));

    // Get this out of the for loop if not used:
    bool has_additional_outputs = false;
    if (freq_optr)
        has_additional_outputs = true;
    else if (phase_optr)
        has_additional_outputs = true;
    else if (error_optr)
        has_additional_outputs = true;

    for (int i = 0; i < noutput_items; i++) {
        if (!tags.empty()) {
            if (tags[0].offset - nitems_read(0) == (size_t)i) {
                d_phase = (float)pmt::to_double(tags[0].value);
                tags.erase(tags.begin());
            }
        }

        const gr_complex nco_out = gr_expj(-d_phase);

        gr::fast_cc_multiply(optr[i], iptr[i], nco_out);

        // EXPENSIVE LINE with function pointer, switch was about 20% faster in testing.
        // Left in for logic justification/reference. d_error = phase_detector_2(optr[i]);
        switch (d_order) {
        case 2:
            if (d_use_snr)
                d_error = phase_detector_snr_2(optr[i]);
            else
                d_error = phase_detector_2(optr[i]);
            break;
        case 4:
            if (d_use_snr)
                d_error = phase_detector_snr_4(optr[i]);
            else
                d_error = phase_detector_4(optr[i]);
            break;
        case 8:
            if (d_use_snr)
                d_error = phase_detector_snr_8(optr[i]);
            else
                d_error = phase_detector_8(optr[i]);
            break;
        }
        d_error = gr::branchless_clip(d_error, 1.0);

        advance_loop(d_error);
        phase_wrap();
        frequency_limit();

        if (has_additional_outputs) {
            if (freq_optr)
                freq_optr[i] = d_freq;
            if (phase_optr)
                phase_optr[i] = d_phase;
            if (error_optr)
                error_optr[i] = d_error;
        }
    }

    return noutput_items;
}

void costas_loop_cc_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    // Getters
    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<costas_loop_cc, float>(alias(),
                                                         "error",
                                                         &costas_loop_cc::error,
                                                         pmt::mp(-2.0f),
                                                         pmt::mp(2.0f),
                                                         pmt::mp(0.0f),
                                                         "",
                                                         "Error signal of loop",
                                                         RPC_PRIVLVL_MIN,
                                                         DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<control_loop, float>(alias(),
                                                       "frequency",
                                                       &control_loop::get_frequency,
                                                       pmt::mp(0.0f),
                                                       pmt::mp(2.0f),
                                                       pmt::mp(0.0f),
                                                       "",
                                                       "Frequency Est.",
                                                       RPC_PRIVLVL_MIN,
                                                       DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<control_loop, float>(alias(),
                                                       "phase",
                                                       &control_loop::get_phase,
                                                       pmt::mp(0.0f),
                                                       pmt::mp(2.0f),
                                                       pmt::mp(0.0f),
                                                       "",
                                                       "Phase Est.",
                                                       RPC_PRIVLVL_MIN,
                                                       DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<control_loop, float>(alias(),
                                                       "loop_bw",
                                                       &control_loop::get_loop_bandwidth,
                                                       pmt::mp(0.0f),
                                                       pmt::mp(2.0f),
                                                       pmt::mp(0.0f),
                                                       "",
                                                       "Loop bandwidth",
                                                       RPC_PRIVLVL_MIN,
                                                       DISPTIME | DISPOPTSTRIP)));

    // Setters
    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_set<control_loop, float>(alias(),
                                                       "loop_bw",
                                                       &control_loop::set_loop_bandwidth,
                                                       pmt::mp(0.0f),
                                                       pmt::mp(1.0f),
                                                       pmt::mp(0.0f),
                                                       "",
                                                       "Loop bandwidth",
                                                       RPC_PRIVLVL_MIN,
                                                       DISPNULL)));
#endif /* GR_CTRLPORT */
}

} /* namespace digital */
} /* namespace gr */
