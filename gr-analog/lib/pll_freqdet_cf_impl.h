/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_PLL_FREQDET_CF_IMPL_H
#define INCLUDED_ANALOG_PLL_FREQDET_CF_IMPL_H

#include <gnuradio/analog/pll_freqdet_cf.h>

namespace gr {
namespace analog {

class pll_freqdet_cf_impl : public pll_freqdet_cf
{
public:
    pll_freqdet_cf_impl(float loop_bw, float max_freq, float min_freq);
    ~pll_freqdet_cf_impl() override;

    void set_loop_bandwidth(float bw) override;
    void set_damping_factor(float df) override;
    void set_alpha(float alpha) override;
    void set_beta(float beta) override;
    void set_frequency(float freq) override;
    void set_phase(float phase) override;
    void set_min_freq(float freq) override;
    void set_max_freq(float freq) override;

    float get_loop_bandwidth() const override;
    float get_damping_factor() const override;
    float get_alpha() const override;
    float get_beta() const override;
    float get_frequency() const override;
    float get_phase() const override;
    float get_min_freq() const override;
    float get_max_freq() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PLL_FREQDET_CF_IMPL_H */
