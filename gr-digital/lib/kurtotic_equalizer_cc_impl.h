/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_IMPL_H
#define INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_IMPL_H

#include <gnuradio/digital/kurtotic_equalizer_cc.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/math.h>
#include <complex>
#include <stdexcept>

namespace gr {
namespace digital {

class kurtotic_equalizer_cc_impl : public kurtotic_equalizer_cc,
                                   filter::kernel::fir_filter_ccc
{
private:
    std::vector<gr_complex> d_new_taps;
    gr_complex d_error;

    float d_mu;
    float d_p, d_m;
    gr_complex d_q, d_u;
    float d_alpha_p, d_alpha_q, d_alpha_m;

    gr_complex sign(gr_complex x)
    {
        float re = (float)(x.real() >= 0.0f);
        float im = (float)(x.imag() >= 0.0f);
        return gr_complex(re, im);
    }

protected:
    gr_complex error(const gr_complex& out) override
    {
        // p = E[|z|^2]
        // q = E[z^2]
        // m = E[|z|^4]
        // u = E[kurtosis(z)]

        float nrm = std::norm(out);
        gr_complex cnj = std::conj(out);
        float epsilon_f = 1e-12;
        gr_complex epsilon_c = gr_complex(1e-12, 1e-12);

        d_p = (1 - d_alpha_p) * d_p + (d_alpha_p)*nrm + epsilon_f;
        d_q = (1 - d_alpha_q) * d_q + (d_alpha_q)*out * out + epsilon_c;
        d_m = (1 - d_alpha_m) * d_m + (d_alpha_m)*nrm * nrm + epsilon_f;
        d_u = d_m - 2.0f * (d_p * d_p) - d_q * d_q;

        gr_complex F =
            (1.0f / (d_p * d_p * d_p)) *
            (sign(d_u) * (nrm * cnj - 2.0f * d_p * cnj - std::conj(d_q) * out) -
             std::abs(d_u) * cnj);

        float re = gr::clip(F.real(), 1.0);
        float im = gr::clip(F.imag(), 1.0);
        return gr_complex(re, im);
    }

    void update_tap(gr_complex& tap, const gr_complex& in) override
    {
        tap += d_mu * in * d_error;
    }

public:
    kurtotic_equalizer_cc_impl(int num_taps, float mu);
    ~kurtotic_equalizer_cc_impl() override;

    float gain() const override { return d_mu; }

    void set_gain(float mu) override
    {
        if (mu < 0)
            throw std::out_of_range(
                "kurtotic_equalizer_cc_impl::set_gain: Gain value must be >= 0");
        d_mu = mu;
    }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_IMPL_H */
