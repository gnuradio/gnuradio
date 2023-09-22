/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_IMPL_H
#define INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_IMPL_H

#include <gnuradio/digital/clock_recovery_mm_ff.h>
#include <gnuradio/filter/mmse_fir_interpolator_ff.h>

namespace gr {
namespace digital {

class clock_recovery_mm_ff_impl : public clock_recovery_mm_ff
{
public:
    clock_recovery_mm_ff_impl(float omega,
                              float gain_omega,
                              float mu,
                              float gain_mu,
                              float omega_relative_limi);
    ~clock_recovery_mm_ff_impl() override;

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
    float d_gain_mu;              // gain for adjusting mu
    float d_omega;                // nominal frequency
    float d_gain_omega;           // gain for adjusting omega
    float d_omega_relative_limit; // used to compute min and max omega
    float d_omega_mid;            // average omega
    float d_omega_lim;            // actual omega clipping limit

    float d_last_sample;
    filter::mmse_fir_interpolator_ff d_interp;

    bool d_verbose;

    float slice(float x) { return x < 0 ? -1.0F : 1.0F; }
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_IMPL_H */
