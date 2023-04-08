/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_AGC_FF_IMPL_H
#define INCLUDED_ANALOG_AGC_FF_IMPL_H

#include <gnuradio/analog/agc_ff.h>

namespace gr {
namespace analog {

class agc_ff_impl : public agc_ff, kernel::agc_ff
{
public:
    agc_ff_impl(float rate = 1e-4,
                float reference = 1.0,
                float gain = 1.0,
                float max_gain = 0.0);
    ~agc_ff_impl() override;

    float rate() const override { return kernel::agc_ff::rate(); }
    float reference() const override { return kernel::agc_ff::reference(); }
    float gain() const override { return kernel::agc_ff::gain(); }
    float max_gain() const override { return kernel::agc_ff::max_gain(); }

    void set_rate(float rate) override { kernel::agc_ff::set_rate(rate); }
    void set_reference(float reference) override
    {
        kernel::agc_ff::set_reference(reference);
    }
    void set_gain(float gain) override { kernel::agc_ff::set_gain(gain); }
    void set_max_gain(float max_gain) override { kernel::agc_ff::set_max_gain(max_gain); }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_AGC_FF_IMPL_H */
