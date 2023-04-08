/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_AGC_CC_IMPL_H
#define INCLUDED_ANALOG_AGC_CC_IMPL_H

#include <gnuradio/analog/agc_cc.h>

namespace gr {
namespace analog {

class agc_cc_impl : public agc_cc, kernel::agc_cc
{
public:
    agc_cc_impl(float rate = 1e-4,
                float reference = 1.0,
                float gain = 1.0,
                float max_gain = 0.0);
    ~agc_cc_impl() override;

    float rate() const override { return kernel::agc_cc::rate(); }
    float reference() const override { return kernel::agc_cc::reference(); }
    float gain() const override { return kernel::agc_cc::gain(); }
    float max_gain() const override { return kernel::agc_cc::max_gain(); }

    void set_rate(float rate) override { kernel::agc_cc::set_rate(rate); }
    void set_reference(float reference) override
    {
        kernel::agc_cc::set_reference(reference);
    }
    void set_gain(float gain) override { kernel::agc_cc::set_gain(gain); }
    void set_max_gain(float max_gain) override { kernel::agc_cc::set_max_gain(max_gain); }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_AGC_CC_IMPL_H */
