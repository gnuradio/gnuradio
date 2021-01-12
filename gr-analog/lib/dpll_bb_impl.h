/* -*- c++ -*- */
/*
 * Copyright 2007,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_DPLL_BB_IMPL_H
#define INCLUDED_ANALOG_DPLL_BB_IMPL_H

#include <gnuradio/analog/dpll_bb.h>

namespace gr {
namespace analog {

class dpll_bb_impl : public dpll_bb
{
private:
    unsigned char d_restart;
    float d_pulse_phase, d_pulse_frequency;
    float d_gain, d_decision_threshold;

public:
    dpll_bb_impl(float period, float gain);
    ~dpll_bb_impl() override;

    void set_gain(float gain) override { d_gain = gain; }
    void set_decision_threshold(float thresh) override { d_decision_threshold = thresh; }

    float gain() const override { return d_gain; }
    float freq() const override { return d_pulse_frequency; }
    float phase() const override { return d_pulse_phase; }
    float decision_threshold() const override { return d_decision_threshold; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_DPLL_BB_IMPL_H */
