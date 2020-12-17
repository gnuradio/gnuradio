/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_CC_IMPL_H
#define INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_CC_IMPL_H

#include <gnuradio/digital/clock_recovery_mm_cc.h>
#include <gnuradio/filter/mmse_fir_interpolator_cc.h>

namespace gr {
namespace digital {

class clock_recovery_mm_cc_impl : public clock_recovery_mm_cc
{
public:
    clock_recovery_mm_cc_impl(float omega,
                              float gain_omega,
                              float mu,
                              float gain_mu,
                              float omega_relative_limi);
    ~clock_recovery_mm_cc_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    float mu() const override { return d_mu; }
    float omega() const override { return d_omega; }
    float gain_mu() const override { return d_gain_mu; }
    float gain_omega() const override { return d_gain_omega; }

    void set_verbose(bool verbose) override { d_verbose = verbose; }
    void set_gain_mu(float gain_mu) override { d_gain_mu = gain_mu; }
    void set_gain_omega(float gain_omega) override { d_gain_omega = gain_omega; }
    void set_mu(float mu) override { d_mu = mu; }
    void set_omega(float omega) override;

private:
    float d_mu;                   // fractional sample position [0.0, 1.0]
    float d_omega;                // nominal frequency
    float d_gain_omega;           // gain for adjusting omega
    float d_omega_relative_limit; // used to compute min and max omega
    float d_omega_mid;            // average omega
    float d_omega_lim;            // actual omega clipping limit
    float d_gain_mu;              // gain for adjusting mu

    gr_complex d_last_sample;
    filter::mmse_fir_interpolator_cc d_interp;

    bool d_verbose;

    gr_complex d_p_2T, d_p_1T, d_p_0T;
    gr_complex d_c_2T, d_c_1T, d_c_0T;

    gr_complex slicer_0deg(gr_complex sample)
    {
        float real = 0.0f, imag = 0.0f;

        if (sample.real() > 0.0f)
            real = 1.0f;
        if (sample.imag() > 0.0f)
            imag = 1.0f;
        return gr_complex(real, imag);
    }

    gr_complex slicer_45deg(gr_complex sample)
    {
        float real = -1.0f, imag = -1.0f;
        if (sample.real() > 0.0f)
            real = 1.0f;
        if (sample.imag() > 0.0f)
            imag = 1.0f;
        return gr_complex(real, imag);
    }
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_CC_IMPL_H */
