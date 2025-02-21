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

#include "mmse_resampler_cc_impl.h"
#include "gnuradio/thread/thread.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace filter {

mmse_resampler_cc::sptr mmse_resampler_cc::make(float phase_shift, float resamp_ratio)
{
    return gnuradio::make_block_sptr<mmse_resampler_cc_impl>(phase_shift, resamp_ratio);
}

mmse_resampler_cc_impl::mmse_resampler_cc_impl(float phase_shift, float resamp_ratio)
    : block("mmse_resampler_cc",
            io_signature::make2(1, 2, sizeof(gr_complex), sizeof(float)),
            io_signature::make(1, 1, sizeof(gr_complex))),
      d_mu(phase_shift),
      d_delta_mu(resamp_ratio)
{
    if (resamp_ratio <= 0)
        throw std::out_of_range("resampling ratio must be > 0");
    if (phase_shift < 0 || phase_shift > 1)
        throw std::out_of_range("phase shift ratio must be > 0 and < 1");

    set_inverse_relative_rate(d_delta_mu);
    message_port_register_in(pmt::intern("msg_in"));
    set_msg_handler(pmt::intern("msg_in"),
                    [this](pmt::pmt_t msg) { this->handle_msg(msg); });
}

void mmse_resampler_cc_impl::handle_msg(pmt::pmt_t msg)
{
    if (!pmt::is_dict(msg))
        return;
    // set resamp_ratio or mu by message dict
    if (pmt::dict_has_key(msg, pmt::intern("resamp_ratio"))) {
        set_resamp_ratio(pmt::to_float(pmt::dict_ref(
            msg, pmt::intern("resamp_ratio"), pmt::from_float(resamp_ratio()))));
    }
    if (pmt::dict_has_key(msg, pmt::intern("mu"))) {
        set_mu(
            pmt::to_float(pmt::dict_ref(msg, pmt::intern("mu"), pmt::from_float(mu()))));
    }
}

void mmse_resampler_cc_impl::forecast(int noutput_items,
                                      gr_vector_int& ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++) {
        ninput_items_required[i] =
            (int)ceil((noutput_items * d_delta_mu) + d_resamp.ntaps()) + d_delta_idx;
    }
}

int mmse_resampler_cc_impl::general_work(int noutput_items,
                                         gr_vector_int& ninput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];
    const auto max_input_index = ninput_items[0] - d_resamp.ntaps();

    add_item_tag(0, nitems_written(0), pmt::mp("work"), pmt::mp(""));
    gr::thread::scoped_lock lock(d_setter_mutex);
    int idx_in = 0;  // input index
    int idx_out = 0; // output index

    while (idx_out < noutput_items &&
           static_cast<unsigned int>(idx_in + d_delta_idx) < max_input_index) {
        idx_in += d_delta_idx;
        out[idx_out++] = d_resamp.interpolate(&in[idx_in], static_cast<float>(d_mu));
        if (ninput_items.size() == 2) {
            d_delta_mu = static_cast<double>(
                reinterpret_cast<const float*>(input_items[1])[idx_in]);
        }
        const double s = d_mu + d_delta_mu;
        const double f = floor(s);
        d_mu = s - f;
        d_delta_idx = (int)f;
        gr::tag_t tag;
        tag.offset = nitems_written(0) + idx_out;
        tag.key = pmt::string_to_symbol("rsmpl");
        tag.value = pmt::mp(fmt::format(
            "nir {:>6}\tii {:>6}\td_mu {:2.6g}\ts {:2.6g}\tf {:2.6g}\tincr {:>6d}",
            nitems_read(0),
            idx_in,
            d_mu,
            s,
            f,
            d_delta_idx));
        add_item_tag(0, tag);
    }
    set_inverse_relative_rate(d_delta_mu);
    consume_each(idx_in);

    return idx_out;
}

float mmse_resampler_cc_impl::mu() const { return static_cast<float>(d_mu); }

float mmse_resampler_cc_impl::resamp_ratio() const
{
    return static_cast<float>(d_delta_mu);
}

void mmse_resampler_cc_impl::set_mu(float mu)
{
    // rescale mu increment, but not the index increment
    set_resamp_ratio(d_delta_mu * mu / d_mu);

    gr::thread::scoped_lock lock(d_setter_mutex);
    d_mu = static_cast<double>(mu);
}

void mmse_resampler_cc_impl::set_resamp_ratio(float resamp_ratio)
{
    gr::thread::scoped_lock lock(d_setter_mutex);
    d_delta_mu = static_cast<double>(resamp_ratio);
    set_inverse_relative_rate(d_delta_mu);
}

} /* namespace filter */
} /* namespace gr */
