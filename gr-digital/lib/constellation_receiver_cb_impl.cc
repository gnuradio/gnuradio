/* -*- c++ -*- */
/*
 * Copyright 2011,2012,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <cstddef>
#include <cstdint>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "constellation_receiver_cb_impl.h"
#include <gnuradio/expj.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

#include <stdexcept>

namespace gr {
namespace digital {

#define VERBOSE_COSTAS 0 // Used for debugging phase and frequency tracking

constellation_receiver_cb::sptr constellation_receiver_cb::make(
    constellation_sptr constell, float loop_bw, float fmin, float fmax)
{
    return gnuradio::make_block_sptr<constellation_receiver_cb_impl>(
        constell, loop_bw, fmin, fmax);
}

constellation_receiver_cb_impl::constellation_receiver_cb_impl(
    constellation_sptr constellation, float loop_bw, float fmin, float fmax)
    : block("constellation_receiver_cb",
            io_signature::make(1, 1, sizeof(gr_complex)),
            io_signature::makev(1,
                                5,
                                { sizeof(char),
                                  sizeof(float),
                                  sizeof(float),
                                  sizeof(float),
                                  sizeof(gr_complex) })),
      blocks::control_loop(loop_bw, fmax, fmin),
      d_constellation(constellation)
{
    if (d_constellation->dimensionality() != 1)
        throw std::runtime_error(
            "This receiver only works with constellations of dimension 1.");

    message_port_register_in(pmt::mp("set_constellation"));
    set_msg_handler(pmt::mp("set_constellation"),
                    [this](pmt::pmt_t msg) { this->handle_set_constellation(msg); });

    message_port_register_in(pmt::mp("rotate_phase"));
    set_msg_handler(pmt::mp("rotate_phase"),
                    [this](pmt::pmt_t msg) { this->handle_rotate_phase(msg); });
}

constellation_receiver_cb_impl::~constellation_receiver_cb_impl() {}

void constellation_receiver_cb_impl::phase_error_tracking(float phase_error)
{
    advance_loop(phase_error);
    phase_wrap();
    frequency_limit();

#if VERBOSE_COSTAS
    d_debug_logger->debug(
        "cl: phase_error: {:f}  phase: {:f}  freq: {:f}  sample: {:f}+j{:f} "
        " constellation: {:f}+j{:f}",
        phase_error,
        d_phase,
        d_freq,
        sample.real(),
        sample.imag(),
        d_constellation->points()[d_current_const_point].real(),
        d_constellation->points()[d_current_const_point].imag());
#endif
}

void constellation_receiver_cb_impl::set_phase_freq(float phase, float freq)
{
    d_phase = phase;
    d_freq = freq;
}

void constellation_receiver_cb_impl::handle_set_constellation(
    pmt::pmt_t constellation_pmt)
{
    if (pmt::is_any(constellation_pmt)) {
        boost::any constellation_any = pmt::any_ref(constellation_pmt);
        constellation_sptr constellation =
            boost::any_cast<constellation_sptr>(constellation_any);
        set_constellation(constellation);
    } else {
        d_logger->error("Received constellation that is not a PMT any; skipping.");
    }
}

void constellation_receiver_cb_impl::handle_rotate_phase(pmt::pmt_t rotation)
{
    if (pmt::is_real(rotation)) {
        const double phase = pmt::to_double(rotation);
        d_phase += phase;
    } else {
        d_logger->error("Received rotation value that is not real; skipping.");
    }
}

void constellation_receiver_cb_impl::set_constellation(constellation_sptr constellation)
{
    d_constellation = constellation;
}

int constellation_receiver_cb_impl::general_work(int noutput_items,
                                                 gr_vector_int& ninput_items,
                                                 gr_vector_const_void_star& input_items,
                                                 gr_vector_void_star& output_items)
{
    const auto in = reinterpret_cast<const gr_complex*>(input_items[0]);
    auto out = reinterpret_cast<std::uint8_t*>(output_items[0]);
    size_t idx = 0;

    std::vector<tag_t> tags;
    auto offset = nitems_read(0);

    get_tags_in_range(tags, 0, offset, offset + ninput_items[0]);
    if (output_items.size() == 5) {
        auto out_err = reinterpret_cast<float*>(output_items[1]);
        auto out_phase = reinterpret_cast<float*>(output_items[2]);
        auto out_freq = reinterpret_cast<float*>(output_items[3]);
        auto out_symbol = reinterpret_cast<gr_complex*>(output_items[4]);

        for (const auto& tag : tags) {
            for (; idx < tag.offset - offset; ++idx) {
                auto nco =
                    gr_expj(d_phase); // NCO value for derotating the current sample
                auto sample = in[idx] * nco; // downconverted symbol

                float phase_error;
                unsigned int sym_value =
                    d_constellation->decision_maker_pe(&sample, &phase_error);
                phase_error_tracking(phase_error); // corrects phase and frequency offsets

                out[idx] = sym_value;
                out_err[idx] = phase_error;
                out_phase[idx] = d_phase;
                out_freq[idx] = d_freq;
                out_symbol[idx] = sample;
            }
            dispatch_msg(tag.key, tag.value);
        }
        for (; idx < static_cast<unsigned int>(noutput_items); ++idx) {
            auto nco = gr_expj(d_phase); // NCO value for derotating the current sample
            auto sample = in[idx] * nco; // downconverted symbol

            float phase_error;
            unsigned int sym_value =
                d_constellation->decision_maker_pe(&sample, &phase_error);
            phase_error_tracking(phase_error); // corrects phase and frequency offsets

            out[idx] = sym_value;
            out_err[idx] = phase_error;
            out_phase[idx] = d_phase;
            out_freq[idx] = d_freq;
            out_symbol[idx] = sample;
        }
    } else {
        for (const auto& tag : tags) {
            for (; idx < tag.offset - offset; ++idx) {
                auto nco =
                    gr_expj(d_phase); // NCO value for derotating the current sample
                auto sample = in[idx] * nco; // downconverted symbol

                float phase_error;
                unsigned int sym_value =
                    d_constellation->decision_maker_pe(&sample, &phase_error);
                phase_error_tracking(phase_error); // corrects phase and frequency offsets

                out[idx] = sym_value;
            }
            dispatch_msg(tag.key, tag.value);
        }
        for (; idx < static_cast<unsigned int>(noutput_items); ++idx) {
            auto nco = gr_expj(d_phase); // NCO value for derotating the current sample
            auto sample = in[idx] * nco; // downconverted symbol

            float phase_error;
            unsigned int sym_value =
                d_constellation->decision_maker_pe(&sample, &phase_error);
            phase_error_tracking(phase_error); // corrects phase and frequency offsets
            out[idx] = sym_value;
        }
    }


    consume_each(noutput_items);
    return noutput_items;
}

void constellation_receiver_cb_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    // Getters
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
