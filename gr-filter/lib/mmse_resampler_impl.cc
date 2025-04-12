/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2010,2012-2013 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "mmse_resampler_impl.h"
#include <gnuradio/filter/mmse_resampler.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include <cmath>
#include <stdexcept>

namespace gr {
namespace filter {

template <typename sample_t>
typename mmse_resampler<sample_t>::sptr mmse_resampler<sample_t>::make(float phase_shift,
                                                                       float resamp_ratio)
{
    return gnuradio::make_block_sptr<mmse_resampler_impl<sample_t>>(phase_shift,
                                                                    resamp_ratio);
}

template <typename sample_t>
mmse_resampler_impl<sample_t>::mmse_resampler_impl(float phase_shift, float resamp_ratio)
    : block("mmse_resampler",
            io_signature::make2(1, 2, sizeof(sample_t), sizeof(float)),
            io_signature::make(1, 1, sizeof(sample_t))),
      d_mu(phase_shift),
      d_delta_mu(resamp_ratio)
{
    if (resamp_ratio <= 0) {
        throw std::out_of_range("resampling ratio must be > 0");
    }
    if (phase_shift < 0 || phase_shift > 1) {
        throw std::out_of_range("phase shift ratio must be > 0 and < 1");
    }

    this->set_inverse_relative_rate(d_delta_mu);
    this->message_port_register_in(pmt::intern("msg_in"));
    this->set_msg_handler(pmt::intern("msg_in"),
                          [this](pmt::pmt_t msg) { this->handle_msg(msg); });
}

template <typename sample_t>
void mmse_resampler_impl<sample_t>::handle_msg(const pmt::pmt_t& msg)
{
    static auto ratio_key = pmt::intern("resamp_ratio");
    static auto mu_key = pmt::intern("mu");
    if (!pmt::is_dict(msg))
        return;
    // set resamp_ratio or mu by message dict
    if (pmt::dict_has_key(msg, ratio_key)) {
        set_resamp_ratio(pmt::to_float(
            pmt::dict_ref(msg, ratio_key, pmt::from_float(resamp_ratio()))));
    }
    if (pmt::dict_has_key(msg, mu_key)) {
        set_mu(pmt::to_float(pmt::dict_ref(msg, mu_key, pmt::from_float(mu()))));
    }
}

template <typename sample_t>
void mmse_resampler_impl<sample_t>::forecast(int noutput_items,
                                             gr_vector_int& ninput_items_required)
{
    for (auto& req : ninput_items_required) {
        req = std::lrint((noutput_items * d_delta_mu) + d_resamp.ntaps());
    }
}

template <typename sample_t>
int mmse_resampler_impl<sample_t>::general_work(int noutput_items,
                                                gr_vector_int& ninput_items,
                                                gr_vector_const_void_star& input_items,
                                                gr_vector_void_star& output_items)
{
    const sample_t* in = (const sample_t*)input_items[0];
    sample_t* out = (sample_t*)output_items[0];
    const bool rate_input = ninput_items.size() == 2;
    const auto in_length =
        rate_input ? std::min(ninput_items[0], ninput_items[1]) : ninput_items[0];
    const auto max_input_index = in_length - d_resamp.ntaps() + 1;

    gr::thread::scoped_lock lock(d_setter_mutex);
    unsigned int idx_in = 0; // input index
    int idx_out = 0;         // output index

    while (idx_out < noutput_items && idx_in < max_input_index) {
        out[idx_out++] = d_resamp.interpolate(&in[idx_in], static_cast<float>(d_mu));
        if (rate_input) {
            d_delta_mu = static_cast<double>(
                reinterpret_cast<const float*>(input_items[1])[idx_in]);
        }
        const double mu_updated = d_mu + d_delta_mu;
        const double integer_part = floor(mu_updated);
        d_mu = mu_updated - integer_part;
        idx_in += static_cast<int>(integer_part);
    }
    if (rate_input) {
        this->set_inverse_relative_rate(d_delta_mu);
    }
    this->consume_each(idx_in);

    return idx_out;
}

template <typename sample_t>
float mmse_resampler_impl<sample_t>::mu() const
{
    return static_cast<float>(d_mu);
}

template <typename sample_t>
float mmse_resampler_impl<sample_t>::resamp_ratio() const
{
    return static_cast<float>(d_delta_mu);
}

template <typename sample_t>
void mmse_resampler_impl<sample_t>::set_mu(float mu)
{
    // rescale mu increment, but not the index increment
    set_resamp_ratio(d_delta_mu * mu / d_mu);

    gr::thread::scoped_lock lock(d_setter_mutex);
    d_mu = static_cast<double>(mu);
}

template <typename sample_t>
void mmse_resampler_impl<sample_t>::set_resamp_ratio(float resamp_ratio)
{
    gr::thread::scoped_lock lock(d_setter_mutex);
    d_delta_mu = static_cast<double>(resamp_ratio);
    this->set_inverse_relative_rate(d_delta_mu);
}

template class mmse_resampler<float>;
template class mmse_resampler<gr_complex>;
} /* namespace filter */
} /* namespace gr */
