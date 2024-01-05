/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "agc3_cc_impl.h"

/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <gnuradio/io_signature.h>
#include <spdlog/fmt/fmt.h>
#include <volk/volk.h>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace gr {
namespace analog {

agc3_cc::sptr agc3_cc::make(float attack_rate,
                            float decay_rate,
                            float reference,
                            float gain,
                            int iir_update_decim,
                            float max_gain)
{
    return gnuradio::make_block_sptr<agc3_cc_impl>(
        attack_rate, decay_rate, reference, gain, iir_update_decim, max_gain);
}

agc3_cc_impl::agc3_cc_impl(float attack_rate,
                           float decay_rate,
                           float reference,
                           float gain,
                           int iir_update_decim,
                           float max_gain)
    : sync_block("agc3_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_reset(true)
{
    set_reference(reference);
    set_attack_rate(attack_rate);
    set_decay_rate(decay_rate);
    set_gain(gain);
    set_max_gain(max_gain);
    test_and_log_value_domain(iir_update_decim, "input power sampling stride");
    d_iir_update_decim = iir_update_decim;
    set_output_multiple(iir_update_decim * 4);
}

agc3_cc_impl::~agc3_cc_impl() {}

void agc3_cc_impl::test_and_log_value_domain(float value, std::string_view description)
{
    if (value <= 0.0f) {
        d_logger->error(
            "Can't set {} that is not strictly positive: {:g}", description, value);
        throw std::domain_error(fmt::format("non-positive {}", description));
    }
}

float agc3_cc_impl::attack_rate() const
{
    gr::thread::scoped_lock guard(d_setter_mutex);
    return d_attack;
}
float agc3_cc_impl::decay_rate() const
{
    gr::thread::scoped_lock guard(d_setter_mutex);
    return d_decay;
}
float agc3_cc_impl::reference() const
{
    gr::thread::scoped_lock guard(d_setter_mutex);
    return d_reference;
}
float agc3_cc_impl::gain() const
{
    gr::thread::scoped_lock guard(d_setter_mutex);
    return d_gain;
}
float agc3_cc_impl::max_gain() const
{
    gr::thread::scoped_lock guard(d_setter_mutex);
    return d_max_gain;
}

void agc3_cc_impl::set_attack_rate(float rate)
{
    test_and_log_value_domain(rate, "attack rate");
    gr::thread::scoped_lock guard(d_setter_mutex);
    d_attack = rate;
}
void agc3_cc_impl::set_decay_rate(float rate)
{
    test_and_log_value_domain(rate, "decay rate");
    gr::thread::scoped_lock guard(d_setter_mutex);
    d_decay = rate;
}
void agc3_cc_impl::set_reference(float reference)
{
    test_and_log_value_domain(reference, "reference");
    gr::thread::scoped_lock guard(d_setter_mutex);
    d_reference = reference;
}
void agc3_cc_impl::set_gain(float gain)
{
    test_and_log_value_domain(gain, "gain");
    gr::thread::scoped_lock guard(d_setter_mutex);
    d_gain = gain;
}
void agc3_cc_impl::set_max_gain(float max_gain)
{
    if (max_gain != 0.0f) {
        test_and_log_value_domain(max_gain, "maximum gain");
    }
    gr::thread::scoped_lock guard(d_setter_mutex);
    d_max_gain = max_gain;
}

int agc3_cc_impl::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    auto in = reinterpret_cast<const gr_complex*>(input_items[0]);
    auto out = reinterpret_cast<gr_complex*>(output_items[0]);
    gr::thread::scoped_lock guard(d_setter_mutex);
    unsigned int index = 0;
    if (d_reset) {
        float magnitude = 0.0f;
        for (unsigned int idx = 0; idx < d_iir_update_decim * 4; ++idx) {
            magnitude += std::abs(in[idx]);
        }
        d_gain = d_reference * (d_iir_update_decim * 4 / magnitude);

        if (d_max_gain > 0.0f) {
            d_gain = std::min(d_gain, d_max_gain);
        }

        // scale output values
        volk_32f_s32f_multiply_32f(reinterpret_cast<float*>(out),
                                   reinterpret_cast<const float*>(in),
                                   d_gain,
                                   d_iir_update_decim * 4 * 2);
        d_reset = false;
        index = d_iir_update_decim * 4;
    }

    // If not on the first run, do the usual IIR update
    for (; index < static_cast<unsigned int>(noutput_items);
         index += d_iir_update_decim) {
        float mag = std::abs(in[index]);
        // check whether the inverse magnitude would be unbounded
        if (std::isnormal(mag)) {
            float rate = (d_reference > d_gain * mag) ? d_decay : d_attack;
            /* we're not using any C inverse sqrt, nor the VOLK function: The memory
             * allocation / calling overhead for VOLK cannot be justified */
            d_gain = d_gain * (1 - rate) + d_reference * rate / mag;
        } else {
            d_gain *= 1 - d_decay;
        }
        // scale output values
        for (auto out_idx = index; out_idx < index + d_iir_update_decim; ++out_idx) {
            out[out_idx] = in[out_idx] * d_gain;
        }
    }

    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */
