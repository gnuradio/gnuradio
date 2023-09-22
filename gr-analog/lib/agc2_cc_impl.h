/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_AGC2_IMPL_CC_H
#define INCLUDED_ANALOG_AGC2_IMPL_CC_H

#include <gnuradio/analog/agc2_cc.h>

namespace gr {
namespace analog {

class agc2_cc_impl : public agc2_cc, kernel::agc2_cc
{
public:
    agc2_cc_impl(float attack_rate = 1e-1,
                 float decay_rate = 1e-2,
                 float reference = 1.0,
                 float gain = 1.0,
                 float max_gain = 0.0);
    ~agc2_cc_impl() override;

    float attack_rate() const override { return kernel::agc2_cc::attack_rate(); }
    float decay_rate() const override { return kernel::agc2_cc::decay_rate(); }
    float reference() const override { return kernel::agc2_cc::reference(); }
    float gain() const override { return kernel::agc2_cc::gain(); }
    float max_gain() const override { return kernel::agc2_cc::max_gain(); }

    void set_attack_rate(float rate) override { kernel::agc2_cc::set_attack_rate(rate); }
    void set_decay_rate(float rate) override { kernel::agc2_cc::set_decay_rate(rate); }
    void set_reference(float reference) override
    {
        kernel::agc2_cc::set_reference(reference);
    }
    void set_gain(float gain) override { kernel::agc2_cc::set_gain(gain); }
    void set_max_gain(float max_gain) override
    {
        kernel::agc2_cc::set_max_gain(max_gain);
    }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_AGC2_CC_IMPL_H */
